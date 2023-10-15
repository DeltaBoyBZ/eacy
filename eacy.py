'''
Copyright 2023 Matthew Peter Smith

This code is provided under an MIT License.
See LICENSE.txt at the root of this Git repository.
'''

import gdb

gdb.execute("set $eacy_rev = 0")

class EACBreak(gdb.Command):
    def __init__(self):
        super(EACBreak, self).__init__("eacbreak", gdb.COMMAND_USER)

    def invoke(self, bp_name, from_tty):
        bp = gdb.Breakpoint(bp_name)
        if(len(bp.locations) == 0):
            print("Failed to create a breakpoint.")
            return
        if(len(bp.locations) == 1):
            return
        opt = 0
        while opt + 1 < len(bp.locations) and bp.locations[opt].source is None:
            opt += 1
        min = bp.locations[opt].source[1]
        for k in range(opt+1, len(bp.locations)):
            if (bp.locations[k].source is None):
                continue
            if(bp.locations[k].source[1] < min):
                min = bp.locations[k].source[1]
                opt = k
        for k in range(len(bp.locations)):
            if k != opt:
                bp.locations[k].enabled = False
EACBreak()

class EACTBreak(gdb.Command):
    def __init__(self):
        super(EACTBreak, self).__init__("eactbreak", gdb.COMMAND_USER)

    def invoke(self, bp_name, from_tty):
        bp = gdb.Breakpoint(bp_name, temporary = True)
        if(len(bp.locations) == 0):
            print("Failed to create a temporary breakpoint.")
            return
        if(len(bp.locations) == 1):
            return
        opt = 0
        while opt + 1 < len(bp.locations) and bp.locations[opt].source is None:
            opt += 1
        min = bp.locations[opt].source[1]
        for k in range(opt+1, len(bp.locations)):
            if (bp.locations[k].source is None):
                continue
            if(bp.locations[k].source[1] < min):
                min = bp.locations[k].source[1]
                opt = k
        for k in range(len(bp.locations)):
            if k != opt:
                bp.locations[k].enabled = False
EACTBreak()

class EACReload(gdb.Command):
    def __init__(self):
        super(EACReload, self).__init__("eacreload", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        gdb.execute("set $eacy_rev = $eacy_rev + 1")
        gdb.execute("eval \"shell rm -rf .eac.%d\", $eacy_rev")
        gdb.execute("eval \"shell cp -r .eac .eac.%d\", $eacy_rev")
        gdb.execute("call EACRevision($eacy_rev)")
        # gdb.execute("set reload = 1")


EACReload()
