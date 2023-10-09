#!/usr/bin/python3
import os
import argparse
import subprocess
import git
from colorama import Fore
parser = argparse.ArgumentParser(prog="ConfigBuild",
                                 description="Configures and builds the project")


parser.add_argument('-bb','--build_behaviour',choices=['Release','Debug'],required=True)
parser.add_argument('-target','--target',choices=['NUCLEO','BOARD'],required=True)
parser.add_argument('-eth','--ethernet_config',choices=['ON','OFF'],required=True)

args = parser.parse_args()

class ConfigBuild:
    def find_repo_root(self):
        current_directory = str("/opt/ST-LIB/tools")
        repo = git.Repo(current_directory, search_parent_directories=True)
        git_root = repo.git.rev_parse("--show-toplevel")
        self.repo_root = git_root
        print("\nSTLIB REPO: " + self.repo_root)
    def __init__(self):
        self.buildconfig = args.build_behaviour
        self.target = args.target
        self.ethernet = args.ethernet_config
        self.output_dir = self.buildconfig         
        self.find_repo_root()
        self.printConfiguration()

    def find_file(self,search_path):
        for root, _, files in os.walk(search_path):
            for file in files:
                if file.endswith(".a"):
                    return file
    def build(self):
        try:
            os.makedirs(self.repo_root + "/build/" + self.output_dir)
        except FileExistsError:
            pass

        output = self.repo_root + "/build/" + self.output_dir
        if self.buildconfig == "Release":
            if self.target == "BOARD":
                if self.ethernet == "ON":
                    subprocess.call(["cmake", self.repo_root, "-B", output, "-DRELEASE=TRUE","-DNUCLEO=FALSE", "-DETHERNET=TRUE"])
                else:
                    subprocess.call(["cmake", self.repo_root, "-B", output, "-DRELEASE=TRUE","-DNUCLEO=FALSE", "-DETHERNET=FALSE"])
            else:
                if self.ethernet == "ON":
                    subprocess.call(["cmake", self.repo_root, "-B", output, "-DRELEASE=TRUE","-DNUCLEO=TRUE", "-DETHERNET=TRUE"])
                else:
                    subprocess.call(["cmake", self.repo_root, "-B", output, "-DRELEASE=TRUE","-DNUCLEO=TRUE", "-DETHERNET=FALSE"])
        else:
            if self.target == "BOARD":
                if self.ethernet == "ON":
                    subprocess.call(["cmake", self.repo_root, "-B", output, "-DRELEASE=FALSE", "-DNUCLEO=FALSE", "-DETHERNET=TRUE"])
                else:
                    subprocess.call(["cmake", self.repo_root, "-B", output, "-DRELEASE=FALSE", "-DNUCLEO=FALSE", "-DETHERNET=FALSE"])
            else:
                if self.ethernet == "ON":
                    subprocess.call(["cmake", self.repo_root, "-B", output, "-DRELEASE=FALSE", "-DNUCLEO=TRUE", "-DETHERNET=TRUE"])
                else:
                    subprocess.call(["cmake", self.repo_root, "-B", output, "-DRELEASE=FALSE", "-DNUCLEO=TRUE", "-DETHERNET=FALSE"])                                         
        threads = os.cpu_count()
        print( Fore.BLUE +  "\n\nCalling make with {} threads\n\n".format(threads))
        retval = subprocess.call(["make","-j",str(threads),"-C", output])
        if retval != 0:
            print(Fore.RED + "ERRORS OCCURED\n")
            raise Exception("error invoking make")
        
        print(Fore.GREEN + "\nBuild completed successfully!!\n" + Fore.YELLOW)
        try:
            os.makedirs(self.repo_root + "/build/" + self.output_dir + "/lib")
        except FileExistsError:
            pass
        lib_ori = self.repo_root + "/build/" + self.output_dir  + "/" + self.find_file(self.repo_root + "/build/" + self.output_dir)
        lib_dir = self.repo_root + "/build/" + self.output_dir + "/lib"
        subprocess.call(["mv",lib_ori,lib_dir])
        self.printConfiguration()
        exit()


    def printConfiguration(self):
        print(Fore.CYAN +"\n\tConfiguration used:\n")
        print(Fore.CYAN +"\t\tBuild configuration: [DEBUG, RELEASE]: {} ".format(self.buildconfig).upper())
        print(Fore.CYAN +"\t\tTarget [BOARD,NUCLEO]: {} ".format(self.target))
        print(Fore.CYAN +"\t\tEthernet [ON,OFF]: {} \n\n".format(self.ethernet))          



obj = ConfigBuild()
obj.build()



