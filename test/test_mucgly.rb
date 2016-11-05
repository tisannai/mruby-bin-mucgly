require 'test/unit'


BIN = "../../../bin/mucgly"


# Test execution routine.
def runTest( cmdopts, test, sep = ".rx.txt" )
    rf = "result/#{test}.txt"
    gf = "golden/#{test}.txt"

    # system( "export RUBYLIB=../lib; #{BIN} -i -m #{cmdopts} -f #{test}#{sep} -o #{rf}" )
    cmd = "#{BIN} -i -m #{cmdopts} -f #{test}#{sep} -o #{rf}"
    system cmd
#    puts cmd
#    exit

    if false
    #if true
        # Populate golden files after inspection.
        system( "cp #{rf} #{gf}" )
    end

    assert( system( "diff #{rf} #{gf}" ), "FAILED: diff #{rf} #{gf}" )
end


class MucglyTest < Test::Unit::TestCase

    def setup
        # Dir.chdir( 'test' )
        FileUtils.mkdir_p "result"
    end

    def teardown
        # Dir.chdir( '..' )
    end


    def test_basic() runTest( "", "test_basic" ); end
    def test_specials_cli() runTest( "-b @ -e @ -s /", "test_specials_cli" ); end
    def test_specials_cmd() runTest( "", "test_specials_cmd" ); end

    def test_file_ctrl()
        rf1 = "result/test_file_ctrl.txt"
        rf2 = "result/test_redirect.txt"
        gf1 = "golden/test_file_ctrl.txt"
        gf2 = "golden/test_redirect.txt"
        system( "export MUCGLY=./test_include.rb; #{BIN} -m -f test_file_ctrl.rx.txt -o #{rf1}" )
        assert( system( "diff #{rf1} #{gf1}" ), "FAILED: diff #{rf1} #{gf1}" )
        assert( system( "diff #{rf2} #{gf2}" ), "FAILED: diff #{rf2} #{gf2}" )
    end

    def test_block() runTest( "", "test_block" ); end

    def test_multihook() runTest( "", "test_multihook", ".rx.c" ); end

    def test_multihook_cli() runTest( "-u '/*..' -u '..*/' -u '_L/*' -u '_J*/'", "test_multihook_cli", ".rx.c" ); end

    def test_langmode() runTest( "-b '/*-<' -e '>-*/'", "test_langmode", ".rx.c" ); end

end
