/**
 * @file   mucgly.c
 * @author Tero Isannainen <tero.isannainen@gmail.com>
 * @date   Wed Oct 26 20:16:46 2016
 * 
 * @brief  Inline macor processor.
 * 
 * See README.rdoc for full documentation of Mucgly.
 */

#include <stdlib.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/proc.h>
#include <mruby/compile.h>
#include <mruby/string.h>

#include <mucgly_mod.h>




/* ------------------------------------------------------------
 * Main program:
 * ------------------------------------------------------------ */


/**
 * Generate outfile name from infile by removing the outfile_tag from
 * infile name.
 *
 * @param infile      Input file name.
 * @param outfile_tag Tag to remove from infile to get outfile.
 *
 * @return Outfile name.
 */
gchar* infile_to_outfile( gchar* infile, gchar* outfile_tag )
{
  gchar* tag = NULL;
  gchar* ret;

  if ( !outfile_tag )
    return NULL;

  if ( g_strrstr( infile, outfile_tag ) )
    {
      ret = g_strdup( infile );
      tag = g_strrstr( ret, outfile_tag );
      g_stpcpy( tag, tag + strlen( outfile_tag ) );
      return ret;
    }
  else
    {
      return NULL;
    }
}


/**
 * Mucgly entrypoint.
 *
 * @param argc Arg count.
 * @param argv Arg values.
 */
void mucgly_main( int argc, char** argv )
{

  /* Setup default hooks etc. */
  stack_default = sf_new( NULL, NULL );


  /* ------------------------------------------------------------
   * Command line parsing:
   * ------------------------------------------------------------ */

  gchar** opt_files = NULL;
  gchar** opt_configs = NULL;
  gchar* opt_output = NULL;
  gchar* opt_genout = NULL;
  gchar** opt_cli_cmds = NULL;
  gchar* opt_beg_sep = NULL;
  gchar* opt_end_sep = NULL;
  gchar* opt_esc_sep = NULL;
  gchar* opt_all_sep = NULL;
  gchar** opt_multi_sep = NULL;
  gboolean opt_module = FALSE;
  gboolean opt_interact = FALSE;
  gboolean opt_no_init = FALSE;


  GOptionEntry opts_table[] =
    {
      { "files", 'f', 0, G_OPTION_ARG_FILENAME_ARRAY, &opt_files,
        "Input file(s) (default: stdin).", NULL },

      { "configs", 'c', 0, G_OPTION_ARG_FILENAME_ARRAY, &opt_configs,
        "Ruby configuration file(s).", NULL },

      { "output", 'o', 0, G_OPTION_ARG_FILENAME, &opt_output,
        "Output file(s) (default: stdout).", NULL },

      { "genout", 'g', 0, G_OPTION_ARG_STRING, &opt_genout,
        "Generate output file names (remove arg from input filename).", NULL },

      { "cli_cmds", 'l', 0, G_OPTION_ARG_STRING_ARRAY, &opt_cli_cmds,
        "Command line configuration(s).", NULL },

      { "beg_sep", 'b', 0, G_OPTION_ARG_STRING, &opt_beg_sep,
        "Set hookbeg separator (default: '-<').", NULL },

      { "end_sep", 'e', 0, G_OPTION_ARG_STRING, &opt_end_sep,
        "Set hookend separator (default: '>-').", NULL },

      { "esc_sep", 's', 0, G_OPTION_ARG_STRING, &opt_esc_sep,
        "Set hookesc separator (default: '\\').", NULL },

      { "all_sep", 'a', 0, G_OPTION_ARG_STRING, &opt_all_sep,
        "Set all separators.", NULL },

      { "multi_sep", 'u', 0, G_OPTION_ARG_STRING_ARRAY, &opt_multi_sep,
        "Set multiple beg/end hook pairs.", NULL },

      { "module", 'm', 0, G_OPTION_ARG_NONE, &opt_module,
        "Include Mucgly module at startup.", NULL },

      { "interact", 'i', 0, G_OPTION_ARG_NONE, &opt_interact,
        "Flush output for convenient interaction.", NULL },

      { "no_init", 'n', 0, G_OPTION_ARG_NONE, &opt_no_init,
        "No init file (skip MUCGLY env var).", NULL },

      { NULL }
    };


  GError* opts_error = NULL;
  GOptionContext* opts;

  opts = g_option_context_new( "- Inline Macro processor" );
  g_option_context_add_main_entries( opts, opts_table, NULL );
  if ( !g_option_context_parse( opts, &argc, &argv, &opts_error ) )
    {
      g_print( "mucgly: option parsing failed: %s\n", opts_error->message );
      exit( EXIT_FAILURE );
    }


  /* Create initial input state. */
  pstate_t* ps = ps_new( opt_output );
  ruby_ps = ps;

  ps->mrb = mrb_open();

  /* Read init file if existing. */
  if ( !opt_no_init )
    {
      gchar* init;
      if ( ( init = (gchar*) g_environ_getenv( g_get_environ(), "MUCGLY" ) ) )
        {
          if ( !g_file_test( init, G_FILE_TEST_EXISTS ) )
            mucgly_warn( ps_current_file(ps),
                         "MUCGLY env set, but the file \"%s\" does not exist!",
                         init );
          else
            ps_load_ruby_file( ps, init );
        }
    }


  /* Include Mucgly module to flatten the namespace. */
  if ( opt_module )
    mrb_load_string( ps->mrb, "include Mucgly" );


  /* Read Ruby configuration files. */
  if ( opt_configs )
    {
      while ( *opt_configs )
        {
          ps_load_ruby_file( ps, *opt_configs );
          opt_configs++;
        }
    }


  /* Process Ruby command line options. */
  if ( opt_cli_cmds )
    {
      while ( *opt_cli_cmds )
        {
          ps_eval_ruby_str( ps, *opt_cli_cmds, FALSE, "cli" );
          opt_cli_cmds++;
        }
    }


  /* Set hooks. */

  if ( opt_beg_sep )
    sf_set_hook( stack_default, hook_beg, opt_beg_sep );

  if ( opt_end_sep )
    sf_set_hook( stack_default, hook_end, opt_end_sep );

  if ( opt_esc_sep )
    sf_set_hook( stack_default, hook_esc, opt_esc_sep );

  if ( opt_all_sep )
    {
      sf_set_hook( stack_default, hook_beg, opt_all_sep );
      sf_set_hook( stack_default, hook_end, opt_all_sep );
      sf_set_hook( stack_default, hook_esc, opt_all_sep );
    }

  /* Set multi-hooks. */
  if ( opt_multi_sep )
    {
      int cnt = 0;

      /* First check that we have even number of hooks. */
      while ( opt_multi_sep[ cnt ] ) cnt++;

      if ( ( cnt % 2 ) != 0 )
        {
          g_print( "mucgly: Must have even number of multi hook args, had %d!\n", cnt );
          exit( EXIT_FAILURE );
        }

      cnt = 0;
      while ( opt_multi_sep[ cnt ] )
        {
          sf_multi_hook( stack_default,
                         opt_multi_sep[ cnt ],
                         opt_multi_sep[ cnt+1 ],
                         NULL );
          cnt += 2;
        }
    }


  if ( opt_interact )
    /* Flush output immediately in interactive mode. */
    ps->flush = TRUE;


  /* Process input files. */
  if ( opt_files )
    {
      /* Named files. */
      while ( *opt_files )
        {
          ps_process_file( ps,
                           *opt_files,
                           infile_to_outfile(
                             *opt_files,
                             opt_genout )
                           );
          opt_files++;
        }
    }
  else
    {
      /* Process STDIN. */
      ps_process_file( ps, NULL, NULL );
    }

  mrb_close( ps->mrb );
  ps->mrb = mrb_open();

  ps_rem( ps );

}


#ifndef MUCGLY_LIB
/**
 * C-top entry point.
 *
 * @param argc Cli arg count.
 * @param argv Cli arg values.
 *
 * @return Not used.
 */
int main( int argc, char** argv )
{
  /* Enter mucgly. */
  mucgly_main( argc, argv );
}
#endif
