#!/usr/bin/env python

# makes a zip file of a lyx file 
# and the associated graphics and bib files

import os, re, zipfile, sys, datetime, subprocess

class GenerateLyxPackage:

    # obtain the local latex directory
    texmf_home = "/home/bram/texmf"

    def __init__(self, lyx_file_name):

        # obtain the lyx file name
        self.lyx_file_name = lyx_file_name


        f = open(self.lyx_file_name)

        # store the lines of the file as a list
        # note that list() is essential to make a copy
        self.lyx_file = list(f.readlines())
        f.close()

        # writing the zip archive

        # split the filename of the lyx file and use that
        # as the name of the zipfile
        fname_split = os.path.splitext(os.path.basename(lyx_file_name))

        # get the time
        now = datetime.datetime.now()

        # open the zipfile that will be used to export everything to
        self.the_zip = zipfile.ZipFile(fname_split[0] + now.strftime("%d_%m_%Y") + ".zip","w")

        # step 1: update the graphics files in the document 
        # and write them to a zip file
        self.update_graphics_files()

        # step 2: update and add the bib file
        self.update_bib_files()
        
        # step 3: get any locally installed sty files
        self.get_sty_files()

        # step 3: write the lyx file itself to the archive
        self.the_zip.writestr(fname_split[0] + now.strftime("%d_%m_%Y") + fname_split[1],"".join(self.lyx_file))

        self.the_zip.close()
    

    def add_file_to_zip(self, filename_orig, filename_in_zip):

        if not os.path.exists(filename_orig):
            raise AssertionError
        
        self.the_zip.write(filename_orig, filename_in_zip)

    # get the bst file
    def get_bst_file(self):

        bibtexfile = ""

        option_nearest_to_bibfiles = False

        for index, item in enumerate(self.lyx_file):

            # look for the string that says bibfiles as the "options" stating the bst file
            # should be right below it
            if not option_nearest_to_bibfiles and re.match("bibfiles",item) != None:
                option_nearest_to_bibfiles = True

            # match object for the options string
            mo = re.match("^options\s\"(.*?)\"",item)

            # get the nearest option
            if option_nearest_to_bibfiles and mo is not None:
                bibtexfile = mo.group(1)
                break

        if bibtexfile == "":
            print("document does not contain a .bst file")

        return bibtexfile + ".bst"


    def get_sty_files(self):

        # get a list of all the sty packages
        the_list = re.findall("\\\\usepackage{(.*?)}","".join(self.lyx_file))
        the_list = [ i + ".sty" for i in the_list ]


        bibtexfile = self.get_bst_file()

        the_list += [ bibtexfile ] 

        # try to find them in the local texmf directory
        for dirpath, dirnames, filenames in os.walk(self.texmf_home):

            # go through the files
            for file in filenames:

                # yes, file exists
                # get move it to the zip
                if file in the_list:
                    print("adding " + dirpath + "/" + file)
                    self.add_file_to_zip(dirpath + "/" + file, file)


    def update_graphics_files(self):

        # loop through the lines and pick out
        # the filename components
        for i in range(0,len(self.lyx_file)):
            mo = re.search("\s+filename\s(.*)",self.lyx_file[i])
            
            if mo != None:
                graphics_name = mo.group(1)

                if not os.path.exists(graphics_name):
                    raise AssertionError

                self.lyx_file[i] = "\tfilename " + os.path.basename(graphics_name) + "\n"

                self.add_file_to_zip(graphics_name, os.path.basename(graphics_name))


    # now update the bib files
    def update_bib_files(self):
        
        for i in range(0,len(self.lyx_file)):
            mo = re.search("bibfiles\s\"(.*)\"",self.lyx_file[i])

            if mo is not None:
                bibfile_name = mo.group(1)

                bibfile_name_ext = bibfile_name.strip() + ".bib"

                if not os.path.exists(bibfile_name_ext):
                    raise AssertionError

                self.lyx_file[i] = "bibfiles \"" + os.path.basename(bibfile_name) + "\"\n"

                self.add_file_to_zip(bibfile_name_ext, os.path.basename(bibfile_name_ext))


a = GenerateLyxPackage(sys.argv[1])
