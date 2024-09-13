#!/usr/bin/python3
import os
import argparse
import subprocess
import git
import platform
from colorama import Fore

## thanks Windows :)
def replace_forward_slashes(input_string):
    result_string = input_string.replace('/', '\\')
    return result_string
parser = argparse.ArgumentParser(prog="ConfigBuild",
                                 description="Configures and builds the project")

is_windows = "Windows" in platform.system()
move_cmd = "move" if is_windows else "mv"
python_interpreter = "python" if is_windows else "python3"

stlib_path = os.environ.get('STLIB_PATH')
if not stlib_path:
    raise Exception("STLIB_PATH ENV VARIABLE IS NOT SET, \n\t check the readme")


parser.add_argument('-bb','--build_behaviour',choices=['Release','Debug'],required=True)
parser.add_argument('-target','--target',choices=['NUCLEO','BOARD'],required=True)
parser.add_argument('-eth','--ethernet_config',choices=['ON','OFF'],required=True)

args = parser.parse_args()

class ConfigBuild:
    def find_repo_root(self):
        current_directory = os.path.join(stlib_path, "tools")
        repo = git.Repo(current_directory, search_parent_directories=True)
        git_root = repo.git.rev_parse("--show-toplevel")
        self.repo_root = git_root
        print("\nSTLIB REPO: " + self.repo_root)
    def __init__(self):
        self.buildconfig = args.build_behaviour
        self.target = args.target
        self.ethernet = args.ethernet_config
        self.output_dir = self.buildconfig   
        ## this will hold all of the args that we pass to cmake
        self.cmake_args = []      
        self.find_repo_root()
        self.printConfiguration()

    def find_file(self,search_path):
        for root, _, files in os.walk(search_path):
            for file in files:
                if file.endswith(".a"):
                    return file
    def build(self):
        try:
            os.makedirs(os.path.join(self.repo_root,"build" ,self.output_dir))
        except FileExistsError:
            pass

        output = os.path.join(self.repo_root,"build" ,self.output_dir)
        self.cmake_args.append("cmake")
        self.cmake_args.append(self.repo_root)
        self.cmake_args.append("-B")
        self.cmake_args.append(output)
        if self.buildconfig == "Release":
            self.cmake_args.append("-DRELEASE=TRUE")
        else:
            self.cmake_args.append("-DRELEASE=FALSE")
        if self.target == "BOARD":
            self.cmake_args.append("-DNUCLEO=FALSE")
        else:
            self.cmake_args.append("-DNUCLEO=TRUE")
        if self.ethernet == "ON":
            self.cmake_args.append("-DETHERNET=TRUE")
        else:
            self.cmake_args.append("-DETHERNET=FALSE")
        ## for some reason windows like to reset back to Visual Studio Generator, make sure we use Unix Makefiles
        self.cmake_args.append("-G")
        self.cmake_args.append("Unix Makefiles")  
        subprocess.call(self.cmake_args)                                  
        threads = os.cpu_count()
        print( Fore.BLUE +  "\n\nCalling make with {} threads\n\n".format(threads))
        retval = subprocess.call(["make","-j",str(threads),"-C", output])
        if retval != 0:
            print(Fore.RED + "ERRORS OCCURED\n")
            raise Exception("error invoking make")
        
        print(Fore.GREEN + "\nBuild completed successfully!!\n" + Fore.YELLOW)
        try:
            os.makedirs(os.path.join(self.repo_root,"build" ,self.output_dir,"lib"))
        except FileExistsError:
            pass
        lib_ori = os.path.join(self.repo_root,"build" ,self.output_dir,self.find_file(self.repo_root + "/build/" + self.output_dir))
        lib_dir = os.path.join(self.repo_root,"build" ,self.output_dir,"lib")
        if is_windows:
            subprocess.call([move_cmd,lib_ori,lib_dir],shell=True)
        else:
            subprocess.call([move_cmd,lib_ori,lib_dir])
        self.printConfiguration()
        exit()


    def printConfiguration(self):
        print(Fore.CYAN +"\n\tConfiguration used:\n")
        print(Fore.CYAN +"\t\tBuild configuration: [DEBUG, RELEASE]: {} ".format(self.buildconfig).upper())
        print(Fore.CYAN +"\t\tTarget [BOARD,NUCLEO]: {} ".format(self.target))
        print(Fore.CYAN +"\t\tEthernet [ON,OFF]: {} \n\n".format(self.ethernet))          
        print(Fore.RED)


obj = ConfigBuild()
obj.build()
