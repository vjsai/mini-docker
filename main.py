# This is a sample Python script.
import os
# Press ⌃R to execute it or replace it with your code.
# Press Double ⇧ to search everywhere for classes, files, tool windows, actions, and settings.
import sys
import subprocess
from cgroups import Cgroup
from cgroups.user import create_user_cgroups
import system
try:
    from pychroot import Chroot
except ImportError:
    print('warning: missing chroot options')


def main():
    if len(sys.argv) < 2:
        print("please provide some arguments")
        return
    elif sys.argv[1] == "run":
        run()


def run():
    command = " ".join(sys.argv[2:])
    print("running command: " + command)
    name = "container"
    user = os.getlogin()
    create_user_cgroups(user)

    # First we create the cgroup and we set it's cpu and memory limits
    cg = Cgroup(name)
    cg.set_cpu_limit(50)  # TODO : get these as command line options
    cg.set_memory_limit(500)

    def in_cgroup():
        try:
            system.unshare(system.CLONE_NEWUTS)
            pid = os.getpid()
            cg = Cgroup(name)

            # add process to cgroup
            cg.add(pid)
        except Exception as e:
            print("Failed to preexecute function")
    subprocess.Popen(command, shell=True, preexec_fn=in_cgroup)


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
