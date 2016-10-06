# source url: http://ucrel.lancs.ac.uk/bncfreq/lists/1_1_all_alpha.txt

# source format: 
# Word = Word type (headword followed by any variant forms) - see pp.4-5
# PoS  = Part of speech (grammatical word class - see pp. 12-13)
# Freq = Rounded frequency per million word tokens (down to a minimum of 10 occurrences of a 
#        lemma per million)- see pp. 5
# Ra   = Range: number of sectors of the corpus (out of a maximum of 100) in which the word 
#        occurs
# Disp = Dispersion value (Juilland's D) from a minimum of 0.00 to a maximum of 1.00.

# goal format (order doesn't matter for usage but will end up being alphabetical):
# [word]    [PoS]   [freq.] [disp.]
# fulfil    Verb   12  0.95
# fulfilled VerbPa  9   0.94
# yacht NoC 14  0.77

# Out file PoS
# Verb      verb, default? (sail)
# VerbUs    verb, only after third-person singular noun (sails), Us = unnamed "s"
# VerbPa    verb, past tense (sailed, written, withdrawn)
# VerbGe    verb/gerund, research on usage rules (sailing)
# NoC       common noun
# NoCPo     common noun, possessive (pig's)
# NoCPl     common noun, plural
# NoP       proper noun (London)
# NoPPl     proper noun, plural (Andersons)
# NoPPo     proper noun, possessive (London's) *added manually
# Adj       adjective, follows or precedes a noun
# Adv       adverb, follows or precedes a verb (angrily)
# Prep      preposition (?)
# Pron      pronoun (she, it)
# Conj      (and, but)
# Num       number (zero)
# Int       ???
# Fore      ???
# Uncl      unclassified

# include (word and all -ing -s etc. variations) if base word usage is > 1

# exclude if:
#   word contains any non-letter chars except for "'" (and ' not at start or end)

# exclude - words but allow them in the final program and treat them as two separate words

# include words of all lengths (length suggestion to be handled by the module)

# for now, all others go in a separate file for analysis. eg. Uncl (unclassified), Fore

# preserve capitalisation

# Proper Noun Categories:
#   -'s     possession  precedes noun

# Adjective Categories:
# (just leave for now, all can be put anywhere afaik


import re


MIN_COUNT = 40
IN_FILE_NAME = "wordlist.txt"
OUT_FILE_NAME = "parsed_wordlist.txt"
NGRAMS_FILE_NAME = "ngrams_top_50000.txt"

acceptableWordRegex = re.compile('^[a-zA-Z][a-z-\']*[a-z]+$')
romanNumeralRegex = re.compile('xi|xl|xv|xx')

addedWordsSet = set()

# set of two letter words
twoLetterWords = {'am', 'an', 'as', 'at', 'be', 'by', 'do', 'go', 'hi', 'if', 'in', 'is', 
        'it', 'my', 'no', 'of', 'oh', 'on', 'or', 'to', 'up', 'us', 'we', 'ya'}

def getNgramsWords():
    inFile = open(NGRAMS_FILE_NAME, 'r')

    lineList = list()
    wordSet = set()

    for line in inFile:
        lineList = line.split()

        wordSet.add(lineList[0])

    return wordSet

# end getNgramsWords()

ngramsWordSet = getNgramsWords()

# manually adds words that are missing or incomplete in the list being parsed
def addPresetWords(wordList):
    # wordEntry = ("is", "Verb", "10875", "0.95")     # same freq, dist as "it"
    # wordList.append(wordEntry)
    # wordEntry = ("be", "VerbPa", "7000", "0.94")
    # wordList.append(wordEntry)
    wordEntry = ("are", "Verb", "5000", "0.88")
    wordList.append(wordEntry)

# end addPresentWords()

# return True if a word should be filtered (ie. not added to the file)
def filterWord(lineList):
    # filter using acceptable word regex
    if not acceptableWordRegex.match(lineList[0]):
        return True

    # filter two-letter words that aren't pre-approved
    if len(lineList[0]) == 2 and lineList[0] not in twoLetterWords:
        return True

    # filter roman numerals
    if lineList[1] == "Num" and romanNumeralRegex.match(lineList[0]):
        return True

    # filter unclassified words
    if lineList[1] == "Uncl":
        return True

    frequency = int(lineList[3])
    dispersion = float(lineList[5])

    # filter uncommon hyphenations
    if "-" in lineList[0] and frequency == 0:
        return True

    # apply more relaxed inclusion rules if the word is a part of the top 50000 words from
    # google ngrams
    if lineList[0] in ngramsWordSet:
        if not lineList[0] in addedWordsSet and dispersion >= 0.50:
            return False
        if (frequency == 0 and dispersion >= 0.65) or (frequency >= 1 and dispersion >= 0.40):
            return False
        #print "In ngrams but excluded: " + lineList[0] + " " + lineList[1]
        #return True

    # use different filtering for proper nouns
    if lineList[1] == "NoP":
        if frequency == 1 and dispersion >= 0.75:
            return False
        if frequency >= 2 and dispersion >= 0.60:
            return False
        if frequency >= 3:
            return False

        return True

    # if word's frequency/dispersion combination is too low, skip it
    if frequency == 0 and dispersion < 0.84:
        #print "                          Questionable: " + lineList[0] + " " + lineList[1]
        return True
    elif frequency == 1 and dispersion < 0.77:
        #print "                          Questionable: " + lineList[0] + " " + lineList[1]
        return True
    elif frequency == 2 and dispersion < 0.62:
        #print "                          Questionable: " + lineList[0] + " " + lineList[1]
        return True
    elif frequency == 3 and dispersion < 0.62:
        #print "                          Questionable: " + lineList[0] + " " + lineList[1]
        return True
    elif frequency >= 4 and dispersion < 0.55:
        #print "                          Questionable: " + lineList[0] + " " + lineList[1]
        return True

    return False

# end filterWord()


def getPossessiveEntry(wordEntry):
    if wordEntry[1] == "NoP":
        frequency = str(int(wordEntry[2]) / 2 + 1) 
        distribution = str("%.2f" % (float(wordEntry[3]) * 0.9))
    if wordEntry[1] == "NoC":
        frequency = str(int(wordEntry[2]) / 4 + 1) 
        distribution = str("%.2f" % (float(wordEntry[3]) * 0.3))

    # handle case where calculated frequency is > original word's frequency
    if int(frequency) > int(wordEntry[2]):
        frequency = wordEntry[2]

    # create the new variation
    newVariation = wordEntry[0] + "'s"

    return [newVariation, wordEntry[1] + "Po", frequency, distribution]

# end getPossessiveEntry()


def getPluralEntry(wordEntry):
    frequency = str(int(wordEntry[2]) / 3 + 1) 
    distribution = str("%.2f" % (float(wordEntry[3]) * 0.4))

    # handle case where calculated frequency is > original word's frequency
    if int(frequency) > int(wordEntry[2]):
        frequency = wordEntry[2]

    # create the new variation
    if wordEntry[0][-1:] == "s" or wordEntry[0][-2:] == "ch" or wordEntry[0][-1:] == "x":
        newVariation = wordEntry[0] + "es"
    else:
        newVariation = wordEntry[0] + "s"

    return [newVariation, "NoPPl", frequency, distribution]

# end getPluralEntry()

# not currently used, depreciated
def getPossessivePluralEntry(wordEntry):
    frequency = str(int(wordEntry[2]) / 5 + 1) 
    distribution = str("%.2f" % (float(wordEntry[3]) * 0.1))
    return [wordEntry[0] + "s", "NoPPl", frequency, distribution]

# end getPossessivePluralEntry()

       
def addVariation(lineList, wordList, originalPoS, originalWord):
    additionalEntries = list()
    wordEntry = [lineList[2], "", lineList[3], lineList[5]]

    # if the variation is actually the original word, no extra processing is required
    if lineList[2] == originalWord:
        wordEntry[1] = originalPoS

        # if the PoS is a proper noun, add the possessive and plural variations
        if wordEntry[1] == "NoP":
            # create artificial frequency and distribution for proper noun variations
            additionalEntries.append(getPossessiveEntry(wordEntry))
            additionalEntries.append(getPluralEntry(wordEntry))

        # if PoS is a common noun, add possessive variation (currently disabled)
        #if wordEntry[1] == "NoC":
            #additionalEntries.append(getPossessiveEntry(wordEntry))

    # if the original part of speech is a noun categorise it as possessive, plural, or other
    elif originalPoS == "NoC":
        if wordEntry[0][-2:] == "\'s":     # possessive noun
            # possessive nouns are created manually so this line can be excluded
            # no possessive common nouns are currently being included in wordList
            return
        elif wordEntry[0][-1:] ==  "s" or wordEntry[0][-3:] == "men":     # plural noun
            wordEntry[1] = originalPoS + "Pl"
        else:
            #if not wordEntry[0][-1:] == ".":
            #print("Unclassified noun: " + wordEntry[0])
            return 

    elif (originalPoS == "Verb"):
        if wordEntry[0][-3:]  == "ing":     # gerund verb
            wordEntry[1] = "VerbGe"
        elif wordEntry[0][-2:] == "ed" or wordEntry[0][-2:] == "en":    # past tense
            wordEntry[1] = "VerbPa"
        elif wordEntry[0][-1:] == "s":     # verb paired with thid-person singluar noun
            wordEntry[1] = "VerbUs"
        else:
            #print("Unclassified verb/suffix: " + wordEntry[0])
            return

    elif (originalPoS == "Adj"):
        wordEntry[1] = "Adj"

    elif (originalPoS == "Num"):
        wordEntry[1] = "Num"

    else:
        #print("Unclassified word: " + wordEntry[0])
        return

    wordList.append(wordEntry)

    for additionalEntry in additionalEntries:
        wordList.append(additionalEntry)
    

# end addVariation()

def addValidWords(wordList):
    inFile = open(IN_FILE_NAME, 'r')

    currentWord = "-"
    baseWord = "-"
    count = 0
    includingVariations = False
    # the original word (that has variations)
    originalWord = ""
    # the original word's PoS
    originalPoS = ""
    variationPoS = ""
    # suffix = ""
    wordEntry = list()
    # countSkipped = 0
    frequency = 0
    dispersion = 0.0

    # manually add the words "is" and "are"
    addPresetWords(wordList)

    # iterates and checks all lines that have a word in the first column
    for line in inFile:
        lineList = line.split()

        # if the line is that of a variation, handle a variation instead of a normal line
        if lineList[0] == '@':
            # if this variation isn't to be included, skip to the next line
            if includingVariations and acceptableWordRegex.match(lineList[2]):
                addVariation(lineList, wordList, originalPoS, originalWord)
            continue
        
        # check if the word should be filtered (not included)
        if (filterWord(lineList)):
            includingVariations = False
            continue
       
        # if the word has variations, prepare to include its variations then continue
        if lineList[2] == "%":
            includingVariations = True
            originalPoS = lineList[1]
            originalWord = lineList[0]
            continue

        # handle the "Ex" word listing for there
        if lineList[0] == "there" and lineList[1] == "Ex":
            wordEntry = (lineList[0], "Pron", lineList[3], lineList[5])
            wordList.append(wordEntry)

            wordEntry = (lineList[0], "NoC", lineList[3], lineList[5])
            wordList.append(wordEntry)

            wordEntry = (lineList[0], "Int", lineList[3], lineList[5])
            wordList.append(wordEntry)

            wordEntry = (lineList[0], "Adj", lineList[3], lineList[5])
            wordList.append(wordEntry)

            continue


        
        # no other conditions have been met so we should be fine to just add the line's data
        # to the wordList
        wordEntry = (lineList[0], lineList[1], lineList[3], lineList[5])
        wordList.append(wordEntry)

        additionalEntries = list()

        # if the PoS is a proper noun, add the possessive and plural variations
        if wordEntry[1] == "NoP":
            # create artificial frequency and distribution for proper noun variations
            additionalEntries.append(getPossessiveEntry(wordEntry))
            additionalEntries.append(getPluralEntry(wordEntry))

        # if PoS is a common noun, add possessive variation
        #if wordEntry[1] == "NoC":
            #additionalEntries.append(getPossessiveEntry(wordEntry))

        for additionalEntry in additionalEntries:
            wordList.append(additionalEntry)

# end addValidWords()


wordList = list()

addValidWords(wordList)

# print the first WORDS_REQUIRED words to file
print("Printing to file: " + str(OUT_FILE_NAME))

outFile = open(OUT_FILE_NAME, 'w')

for wordEntry in wordList:
    outFile.write(wordEntry[0] + '\t' + wordEntry[1] + '\t' + wordEntry[2] + '\t' + 
            wordEntry[3] + '\n')

