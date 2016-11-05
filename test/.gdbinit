#file ../mucgly
file ../../../bin/mucgly
set args -i -m  -f test_specials_cmd.rx.txt

#set args -m -f test_basic.rx.txt
#set args -m -f test_file_ctrl.rx.txt

#break mucgly_debug
#break main

#break ps_push_curhook
#break ps_pop_curhook

#break fs_push_file_delayed
#break mucgly.c:1658

run
