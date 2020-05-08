# This script generates a keywords.txt file for the Arduino IDE by scraping 
# through the codebase, and picking keywords using the following scheme
# that seems to be widely accepted by the Arduino community:
#
# KEYWORD1 for Classes, Datatypes, and C++ keywords
# KEYWORD2 for Methods and functions
# LITERAL1 for constants
#
# Author: Cody Burrows
# Date: 2019-07-08

import argparse
import datetime
import os
import re


# path to the variants\clearcore\Teknic directory
REL_PATH_TEKNIC = os.path.dirname(os.path.abspath(__file__)) + '\\Teknic\\libClearCore\\'
# path to the pins_arduino header file
REL_PATH_PINS_ARDUINO = os.path.dirname(os.path.abspath(__file__)) + '\\variants\\clearcore\\pins_arduino.h'
# I love escaping escapes
PATTERN_PUBLIC_FUNC = re.compile(r'(?!return|operator\b)\b[\w\*&]+ [\w]+\([^(){}]*\)? ?(override ?)?(;|{)?')
PATTERN_PUBLIC_ENUM = re.compile('(typedef )?enum[^{}()]+{')
PATTERN_ENUM_NAME = re.compile('[A-Z0-9_]+n?[A-Z0-9_]*( += (-)?(0x)?([0-9 <>UL]+|[A-Z0-9_]+n?[A-Z0-9_]*)( [+-] [0-9]+)?)?' + 
                               '((,)?(\s*)\n|(,)?(\s*)//((\s*)(\S*)(\s*))*\n)')        
                               # AL 9-24-2019 updated PATTERN_ENUM_NAME to allow inline comments after an enum,
                               # (\s*) allows optionally any amount of whitespace.
PATTERN_DEFINE_DIRECTIVE = re.compile('(#define)(\s*)[A-Z0-9_]*(\s*)(\w|\W)+')
PATTERN_GLOBAL_INSTANCE = re.compile(r'extern [A-Za-z][&A-Za-z0-9]* *&? *(\*const )?(?P<name>[A-Za-z][A-Za-z0-9]*);')

# relative path of keywords.txt file
keywords = "keywords.txt"

# Argument setup
parser = argparse.ArgumentParser(description='Generate keywords.txt from the ClearCore source files.')
parser.add_argument('-S', action='store_true', help='silent mode, no permission request to remove the file')
parser.add_argument('-p', action='store', dest="p", default="", help="relative path to the keywords.txt file")

# print error and die
def error(errString):
    print('Error: ' + errString)
    exit()


# Prompt user for y/n
def confirmYN(msg):
    ans = str(input(msg + '  (y/n) ')).lower().strip()
    return ans[0] == 'y'
    
        
# write a line out to the keywords.txt file
def writeline(f, line):
    f.write(line + '\n')


# find all public functions in the given file
def publicFunctions(f):
    public = False
    inComment = False
    matches = set()
    openedFile = open(f, 'r')
    for line in openedFile:
        if 'public:' in line:
            public = True
        elif 'protected:' in line:
            public = False
        elif 'private:' in line:
            public = False

        if public:
            # Detect beginning of a multi-line comment.
            if not inComment:
                if '/*' in line:
                    inComment = True
            # Detect end of a multi-line comment.
            if inComment:
                if '*/' in line:
                    inComment = False
                    
            x = re.search(PATTERN_PUBLIC_FUNC, line)
            # "Match objects always have a boolean value of True" https://docs.python.org/2/library/re.html
            if x and not inComment:
                wholeMatch = x.string[x.start():x.end()]
                name = wholeMatch.split('(')[0] # gets func name with return ty
                name = name.split(' ')[1] # gets rid of return type
                matches.add(name)
                #print('\tFound: ' + name)
            
    openedFile.close()
    return matches


# find all of the enum names in the given file
def publicEnums(f):
    public = False
    inEnum = False
    inComment = False
    matches = set()
    openedFile = open(f, 'r')
    for line in openedFile:
        if 'public:' in line:
            public = True
            inEnum = False
        elif 'protected:' in line:
            public = False
            inEnum = False
        elif 'private:' in line:
            public = False
            inEnum = False

        if public:
            if inEnum:
                # Detect beginning of a multi-line comment.
                if not inComment:
                    if '/*' in line:
                        inComment = True
                # Detect end of a multi-line comment.
                if inComment:
                    if '*/' in line:
                        inComment = False
                # Detect end of an enum.
                if '}' in line:
                    inEnum = False
            else:
                inEnum = re.search(PATTERN_PUBLIC_ENUM, line) is not None

            if inEnum and not inComment:
                x = re.search(PATTERN_ENUM_NAME, line)
                if x:
                    wholeMatch = x.string[x.start():x.end()]
                    if '=' in wholeMatch:
                        name = wholeMatch.split('=')[0].strip()
                    elif ',' in wholeMatch:
                        name = wholeMatch.split(',')[0].strip()
                    else:
                        name = wholeMatch.split(' ')[0].strip()
                    #print(f'Match: {wholeMatch}')
                    #print(f'Found: {name}')
                    matches.add(name)
            
    openedFile.close()
    return matches

# find all #defines and declare the first two values as literals
# ex:       #define IO3 CLEARCORE_PIN_IO3  -> declare IO3 and CLEARCORE_PIN_IO3
def pinDefines(f):
    define = False
    matches = set()
    with open(f) as fp:
        for line in fp:
            if PATTERN_DEFINE_DIRECTIVE.match(line):
                # this is a hack for pins_arduino.h and not how a standard 
                # define would be handled
                pinDirective = line.split()[1].strip()
                pinEnum = line.split()[2].strip()
                #print('\tFound: ' + pinDirective)
                matches.add(pinDirective)
                #print('\tFound: ' + pinEnum)
                matches.add(pinEnum)
    return matches

# find global connector instances
def globalInstances(f):
    matches = set()
    with open(f) as fp:
        for line in fp:
            if PATTERN_GLOBAL_INSTANCE.match(line):
                global_instance = PATTERN_GLOBAL_INSTANCE.match(line).group('name')
                matches.add(global_instance)
    return matches
            

# start "running" the script
print('Running keywordify ' + datetime.date.today().strftime("%Y-%m-%d"))

# parse arguments
args = parser.parse_args()

# make sure we're starting in the right directory
inc_found = os.path.exists(REL_PATH_TEKNIC + 'inc')
src_found = os.path.exists(REL_PATH_TEKNIC + 'src')
pins_h_found = os.path.exists(REL_PATH_PINS_ARDUINO)
if not inc_found:
    error('Couldn\'t find ' + REL_PATH_TEKNIC + 'inc!')
elif not src_found:
    error('Couldn\'t find ' + REL_PATH_TEKNIC + 'src!')
elif not pins_h_found:
    error("Couldn't find " + REL_PATH_PINS_ARDUINO + "!")
else:
    print('Found ' + REL_PATH_TEKNIC + 'inc and\n' + REL_PATH_TEKNIC + 'src\n')

# update the keywords.txt path with an optionally specified location
keywords = args.p + keywords

# check to see if keywords.txt already exists
if os.path.exists(keywords):
    # the '-S' flag indicates silence, don't ask
    if not args.S:
        if not confirmYN('keywords.txt already exists. Overwrite?'):
           exit()
    os.remove(keywords)

# fall through to start scraping
keyword1s = set()
keyword2s = set()
literal1s = set()
REL_PATH_INC = REL_PATH_TEKNIC + 'inc'
REL_PATH_SRC = REL_PATH_TEKNIC + 'src'

# get all of the classes in ...\Teknic\src (KEYWORD1)
print('Searching for Classes...')
for (_, _, f) in os.walk(REL_PATH_SRC):
    for filename in f:
        if filename.endswith('.cpp'):
            name = os.path.splitext(filename)[0]
            #print('\tFound: ' + name)
            keyword1s.add(name)

# add in Serial0
keyword1s.add("Serial0")

# get all of the global connector instances (KEYWORD1)
print('Searching for global instances...')
for (_,_,f) in os.walk(REL_PATH_INC):
    for filename in f:
        if filename == "ClearCore.h":
            keyword1s = keyword1s.union(globalInstances(REL_PATH_INC + '\\' + filename))

# get all of the public functions (KEYWORD2)
print('Searching for public functions...')
for(_, _, f) in os.walk(REL_PATH_INC):
    for filename in f:
        if filename.endswith('.h'):
            #print('\nLooking at file: ' + filename + '\n')
            keyword2s = keyword2s.union(publicFunctions(REL_PATH_INC + 
                                        '\\' + filename))

# get all of the constants (LITERAL1)
print('Searching for constants...')
for(_, _, f) in os.walk(REL_PATH_INC):
    for filename in f:
        if filename.endswith('.h'):
            #print('\nLooking at file: ' + filename + '\n')
            literal1s = literal1s.union(publicEnums(REL_PATH_INC + 
                                        '\\' + filename))

# get all of the pin mappings (LITERAL1)
print('Looking at pin mapping...')
literal1s = literal1s.union(pinDefines(REL_PATH_PINS_ARDUINO))


# done collecting names
# write out to file
# write a timestamp to the file
keywords_txt = open(keywords, 'w+')
timestamp = datetime.datetime.now()

# write out KEYWORD1 entries
writeline(keywords_txt, '# Classes, Datatypes, and C++ keywords')
# sort alphabetically
keyword1sSorted = sorted(keyword1s)
for fname in keyword1sSorted:
    writeline(keywords_txt, fname + '\t' + 'KEYWORD1')

# write out KEYWORD2 entries
keyword2s = keyword2s - keyword1s
# sort alphabetically
keyword2sSorted = sorted(keyword2s)
writeline(keywords_txt, '\n# Public methods and functions')
for fname in keyword2sSorted:
    writeline(keywords_txt, fname + '\t' + 'KEYWORD2')

# write out LITERAL1 entries (sorted alphabetically)
literal1s = literal1s - keyword2s - keyword1s
# sort alphabetically
literal1sSorted = sorted(literal1s)
writeline(keywords_txt, '\n# Public Constants')
for fname in literal1sSorted:
    writeline(keywords_txt, fname + '\t' + 'LITERAL1')

writeline(keywords_txt, '\n# keywordify done')
keywords_txt.close()
print('keywordify done.')
