#!/usr/bin/python

def gen_entry(intno, target):
    err_code = {8, 10, 11, 12, 13, 14, 17, 30}

    print "isr{}:".format(intno)
    if intno not in err_code:
        print "\tpushl $0"
    print "\tpushl ${}".format(intno)
    print "\tjmp {}".format(target)

def gen_save():
    print "\tpushal"
    print "\tpushl %esp"
    print "\tcld"

def gen_restore():
    print "\taddl $4, %esp"
    print "\tpopal"
    print "\taddl $8, %esp"

def gen_isrs(handler, table_name):
    ints = 256
    target = "common_isr"

    for i in xrange(ints):
        gen_entry(i, target)

    print "\n\t.extern {}".format(handler)
    print "{}:".format(target)
    gen_save()
    print "\tcall {}".format(handler)
    gen_restore()
    print "\tiret"

    print "\n\t.global {}".format(table_name)
    print "{}:".format(table_name)
    for i in xrange(ints):
        print "\t.long isr{}".format(i)

gen_isrs("raw_isr", "raw_isrs_table")
