/* Test that mucgly can help text editor's language mode to be in sync
   with the mucgly macros. */
/*-<+ # Create a function that is inlined with mucgly.
def inline_mult( a, b )
     Mucgly.write "#{a} * #{b}"
end
>-*/
/*-<+:eater @>-*/
/*-<+Mucgly.seteater( "@" )>-*/
/*-<+Mucgly.multihook( [ "rx(", ")", "(" ] )>-*/


int my_func( int a1, int a2 )
{
  int c;

  if ( a1 >= a2 )
    c = a1 - a2;
  else
    c = a2 - a1;

  c = rx( inline_mult( String.new("c"), "a1" )\@" );
\@"
  return c;
}
