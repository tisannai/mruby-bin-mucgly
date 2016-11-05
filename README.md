# Mucgly

## MRuby

This is MRuby version of Mucgly. It requires mruby-bin-mucgly and
mruby-mucgly gems.

Add this to your "build_config.rb", to get Mucgly included:

    conf.gembox "#{root}/mrbgems/mruby-bin-mucgly/mucgly"


## Introduction

Mucgly is a macro expander for inline macros that exist in the middle of body
text. Mucgly is useful for code generation and all kinds of templating
systems. The macros are expected to be mostly regular Ruby code, but a few
special commands are also available.

A very simple example:

    Adding 1 + 3 results to: -<.1+3>-

After macro expansion the results is:

    Adding 1 + 3 results to: 4

By default macro starts with "-<" and ends with ">-". These limiters are
called hooks, hookbeg and hookend, respectively. The code between hooks is
mostly regular Ruby code. The first character is special syntax and the rest
is pure Ruby code. The special character (command) "." is used to print the
Ruby code evaluation value to current output file (stream).

Sample session to execute the above example interactively:

    shell> mucgly
    Adding 1 + 3 results to: -<.1+3>-
    Adding 1 + 3 results to: 4
    <Ctrl-D>
    shell>

Ruby code is always executed in the Ruby interpreters "\<main\>" scope, i.e.
Execution Environment.

Previous example with multiple macros:

    -<@a = 1>--<@b = 3>-\
    Adding 1 + 3 results to: -<Mucgly.write(@a+@b)>-

Result is exactly the same as in the previous execution, but the Ruby code is
distributed into multiple segments. The first macro "-<@a = 1>-" produces no
output, it just sets the variable "@a" to "1". Second macro is similar. The
default "escape" character (hookesc) is "\". When placed before newline
character, it "eats" the newline and nothing is output. Thus the first line
outputs nothing.

The macro on the second line refers to settings from previous macros. Instance
variables has to be used to share data between macro calls (due to "\<main\>"
evaluation). Local variables are not persistent enough. Mucgly module method,
"Mucgly.write", is used to write out the calculation result. Mucgly module is
part of Mucgly.

Typically Mucgly is executed in batch mode, e.g:

    mucgly -f foo.rx.txt -f bar.rx.txt -g .rx

For all command line options, see:

    mucgly -h

## Features

*   User settable hooks to define macro boundaries. Can be set from command
    line, configuration files, or from macro input file.

*   Multiple, concurrent macro start and end limiters (multihook).

*   Compatible with editor's language sensitive mode.

*   Multiple sources for configuration: default config, environment variable,
    command line.

*   Multipass support.

*   Multiple convenience functions for macros to use.

*   Macro file introspection: line number, file name.

*   Output stream redirection (de-muxing).

*   Many special commands: include, source, etc...

*   Fast C-language based macro file processing.



## Applications

*   Replacement for M4 macro processor.

*   Ruby pre-processor.

*   General purpose code generation.

*   Document formatting.

*   Template generation.

*   Etc.



## Special characters

### Hooks

Hooks start and end the macro definition. By default hookbeg is "-<" and
hookend is ">-". These values don't conflict easily with typical macro file
body text.

If literal hook string is required in the output, the hookesc sequence should
be place before the hook. For example "\-<" will produce literal "-<" to the
output. "\" is the default hookesc value.

User can set the hooks to whatever string value desired. The hooks can also
have the same value. However nested macros are not possible then. Hooks can
also be the same as the escape sequence, but this makes usage somewhat
complicated.

### Escape

Default hookesc (escape sequence) is "\". It can be used to escape hooks,
cancel space (" ") output, and cancel newline ("\n") output.

User can set the escape sequence to any character(s).

### Remote escape

hookesc can be used to cancel the newline following the macro. However it is
sometimes convenient to have "clean" line ending, but still cancel the
newline. This holds true for example if language sensitive editor mode should
be respected.

If the first character is "+" in the macro body, the character following the
macro is cancelled from output. "+" is removed silently and the rest of the
macro body is handled normally.

Remote escape is typically used to cancel a newline, since other use cases can
be handeled by other means.

### Single char hooks and escape

If all hooks are single character (including hookesc) and have the same value,
there are some additional rules that are used to identify macro boundaries and
escapes. Let's assume that the selected hook char is "|" (vertical bar).

Escape is detected if "| ", "|\n", or "||" is seen in the input, when outside
macro. Escape is detected within macro if "||" is seen in input.

Hookbeg is detected at "|" if outside macro. Hookend is detected at "|" if
inside macro. Thus if literal "|" is needed inside macro, then "||" must be
used. Also expressing multilevel macros (macro in macro) is not possible,
since hookbeg is never detected within macro. The default hook values can be
used for nested macros (if desired).

### Multiple hooks

Mucgly allows the user to define multiple hookbeg and hookend pairs. This can
be done from command line or from Mucgly module methods.

Any of the possible hookbegs will start an macro, and the corresponding
hookend pair will terminate the macro. Multiple hooks are used as a
replacement for single hookbeg and hookend.

Multihook mode is entered when first multihook pair is added. This can happen
multiple times. If a single hook set is performed (again), singlehook mode is
entered. This can also be used as method for resetting the current set of
multihooks.

### Suspension

Suspension is a feature that helps in implementing macros that are compliant
with the text editor's language sensitive mode.

By default Mucgly macros do not look like part of the host language, e.g. C
language. However macros can be made to emulate the host language.

For example the user might want to make the macro to look like a function call
in the host language.

The macro could be used like this:

    c = rx( inline_mult( "c", "a1" ) );

Here hookbeg is "rx(" and hookend is ")". From the C language point of view,
this looks like two nested function calls. "rx" is called and its argument is
a return value from "inline_mult" function. From Ruby point of view, we want
"inline_mult" to be called as a Ruby method. However, by default the macro
would terminate to the first closing parenthesis, which would cause a syntax
error in the end.

The solution is to declare "(" as suspension character. Suspension character
means that for each occurence of the suspension character, one hookend will be
suspended, i.e. de-activated. Hence the first "(" after starting the macro
with "rx(", will suspend the next ")", i.e. hookend. Since there is only one
"(", there will be only one suspension performed, and the final ")" will
terminate the macro. The suspended hookend will be taken as part of the macro
body.

In order to get suspension, a multihook is declared including a suspension
character/string.

    /*-<+Mucgly.multihook( [ "rx(", ")", "(" ] )>-*/

Suspensions could be replaced with simple escape of the first ")" within the
macro call, but this is inconvenient, and might potentially cause issues with
language mode of the host language.

Suspension mechanism is not bullet proof, and thus does not always guarantee
language mode compatibility. Languages with C based syntax work well since
they use the same style of function argument lists and string conventions as
in Ruby. Other type of languages might not benefit from this feature.

### Eater

Eater is character/string which follows hookesc and designates that the
following char should be removed. This applies to both inside and outside
macros. The reason for adding "extra" characters to input could be for example
to trick the language mode of the host language to change coloring.

Eater can be set with ":eater" or with "Mucgly.seteater". If "Mucgly.seteater"
is passed a nil, the Eater feature is disabled and hookesc behaves as by
default.

## Special commands

In addition to regular Ruby code the macros are allowed to include so called
Special Commands. These commands start with ":" or with "." character.

### Named commands

Example for named command:

    ...
    -<:hook [ ]>-\
    ...

This example would change the hookbeg and hookend to "[" and "]" respectively.
Note that hooks are changed only for the current input file. If an included
input file changes hooks, the includer file still uses the original hooks.

One special command is allowed per macro and it can't be mixed with normal
Ruby code. Command name is separated by ":" in the beginning and by space ("
") in the end. The rest of the macro is taken as argument to the command.

List of ":" style special commands:

include
:   Include reverts the input stream to the file given in the argument.
    Command can be used for example to multiplex multiple files into one
    (example: "-<:include input.txt>-" ).

comment
:   Comment is used to add comments into the macro file. No output is produced
    from comment macros.

block
:   Block output starting with this command, see unblock.

unblock
:   Unblock output starting with this command, see block.

source
:   Source reads a plain Ruby file into the Execution Environment.

hook
:   Sets both hookbeg and hookend to values separated by space. Without space
    hookbeg and hookend are set to same value.

hookbeg
:   Sets hookbeg.

hookend
:   Sets hookend.

hookesc
:   Sets escape sequence.

hookall
:   Sets hookbeg, hookend, and hookesc to same value.

exit
:   Aborts the macro processing (file).


### Quick commands

Special write command starts with "." and is followed by the Ruby value.

For example:

    ... -<.@my_name>- ...

Would write out the value of the "@my_name" variable, thus it is equal to
writing:

    ... -<Mucgly.write @my_name>- ...

The special character "/" can also be used for comment macro, i.e. "/" is same
as ":comment ".

### Multipass

If the macro starts with "#" character, the macro is not excuted. The macro
content is output with one "#" removed from the output, surrounded with the
current macro hooks. If 3 passes are needed, then "##" can be placed to the
initial macro file. Multipass macro files are processes by (shell) piping the
output of one Mucgly process to another.

Multipass can be used for example to create a Table Of Contents. First pass
collects information about document content and second pass will insert the
Table Of Contents for a document.

Example, with functions (insert_toc, header...) defined elsewhere:

    -<#insert_toc>-
    -<header(1, "My header 1")>-
    Paragraph1
    -<header(2, "My header 1.1")>-
    Paragraph2
    -<header(1, "My header 2")>-
    Paragraph3

## Mucgly module methods

Ruby Mucgly module includes methods that are used to interact with Mucgly
program internals.

Methods "write", "puts" produce output to current output file stream.
Hook-commands change the current hook values.

Complete list of Mucgly module methods:

write
:   Write string to current output file without newline.

puts
:   Write string to current output file with newline.

hookbeg
:   Get hookbeg value.

hookend
:   Get hookend value.

hookesc
:   Get hookesc value.

sethook
:   Set hookbeg and hookend values.

sethookbeg
:   Set hookbeg value.

sethookend
:   Set hookend value.

sethookesc
:   Set hookesc value.

multihook
:   Define a set of hookbeg/hookend pairs (even number of strings in array).

ifilename
:   Input file name as String.

ilinenumber
:   Input file line number.

ofilename
:   Output file name as String.

olinenumber
:   Output file line number.

pushinput
:   Makes given file as current input stream. When this stream is runs out
    (EOF) or is closed (with closeinput), the input stream is reverted to
    previous input file. Pushinput is same as ":include" command.

closeinput
:   Close output stream (before EOF is encountered).

pushoutput
:   Makes given file as top output stream. When this stream is closed (with
    closeoutput), the output stream is reverted to previous output file.

closeoutput
:   Close output stream.

block
:   Block output starting with this command, see unblock.

unblock
:   Unblock output starting with this command, see block.


## Command line interface

Please execute:

    mucgly -h

for an overview of the command line options.

The user must use the same option multiple times in order to enter multiple
args of the same class.

### Input and Output files

Input files can be listed to "-f" option. The default input stream for Mucgly
is STDIN.

Output file can be defined with "-o" option. The default output stream for
Mucgly is STDOUT.

If "-g" option is given, the output file list is created by extracting "-g"
option argument value from each input file name.

For example with:

    mucgly -f foo.rx.txt -f bar.rx.txt -g .rx

Two files, foo.txt and bar.txt, would be created. This holds true unless the
output streams are manipulated with macro commands.

## Configuration

Mucgly checks for the existance of the "MUCGLY" environment variable. If the
variable exists, the file in variable value is read in as plain Ruby.

Configuration files can also be given on command line using the "-c" option.
These settings can be used to override the settings above.

Additionally plain Ruby code can be given straight on the command line with
"-l" option.

## More information

Check out the "test" directory within installation for detailed examples about
macro usage.
