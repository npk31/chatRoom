import subprocess
import os
import sys
from pathlib import Path


__dir__ = Path(__file__).resolve().parent
dist = __dir__ / "dist"

commands = [
    f"g++ {__dir__ / "server.cpp"} -o {dist / "sv"}",
    f"g++ {__dir__ / "client.cpp"} -o {dist / "cl"}",
    f"g++ {__dir__ / "conversation.cpp"} -o {dist / "cvs"}",
    f"g++ {__dir__ / "sendMessage.cpp"} -o {dist / "sm"}",
]

for i in range(0, len(commands)):
    print("Compiling... " + str(i + 1) + "/" + str(len(commands)))
    subprocess.Popen(commands[i], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).wait()
    os.system("cls" if os.name == "nt" else "clear")

os.system("cls" if os.name == "nt" else "clear")
print("Initialization complete.")