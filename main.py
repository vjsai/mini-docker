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
    elif sys.argv[1] == "child":
        child()


def run():
    command = " ".join(sys.argv[2:])
    print("running command: " + command + " with pid :" + str(os.getpid()))
    name = "container"
    pid = system.clone(child, system.CLONE_NEWUTS | system.CLONE_NEWNS | system.CLONE_NEWPID, tuple(command))
    _, status = os.waitpid(pid, 0)
    print('{} exited with status {}'.format(pid, status))


def child(*argv):
    child_command = ''.join(argv)
    print("running command: " + child_command + " with pid :" + str(os.getpid()))

    def pre_execute():
        system.sethostname("container")
        os.chroot("dummy_root")

    subprocess.Popen(child_command, preexec_fn=pre_execute)


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
