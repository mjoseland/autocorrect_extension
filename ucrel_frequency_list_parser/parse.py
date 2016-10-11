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

# Parts of speech	(* = no words currently)
#         pspeech  str       description
#		0  Verb      verb, default? (jump, study)
#		1  VerbTPP   verb, third-person present (sails)
#		2  VerbPa    verb, past tense (incl. participle) (sailed, had, rode, ridden)
#		3  VerbGe    verb/gerund, research on usage rules (sailing)
#		4  NoC       common noun (island, ship)
#		5  NoCPl     common noun, plural (islands, dogs)
#		6  NoCPo     *common noun, possessive (pig's)
#		7  NoP       proper noun (London)
#		8  NoPPl     proper noun, plural (Andersons)
#		9  NoPPo     proper noun, possessive (London's)		- added manually
#		10 Adv       adverb, follows or precedes a verb (angrily, in)
#		11 Adj       adjective, follows or precedes a noun (willing, small)
#		12 VMod	     verb modifier/helper (should, might, could)
#		13 DetP      (enough, latter, what)
#		14 Prep      preposition (beyond, according, in)
#		15 Pron      pronoun (she, hers, someone, they)
#		16 Conj      conjunction (and, but, if)
#		17 Int       interjection (yikes, uh-oh)
#		18 Fore      *???                               * not included
#		19 Det	     your, every, the
#		20 Num       number (zero)
#		21 Inf	     single word : to 
#		22 Neg	     single word : not
#		23 Uncl      *unclassified


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

# TODO
#   remove offensive words
#   build separate files for US and UK english (traumatised, traumatized)
#   better integrate frequencies from top_50000_words.txt and wordlist.txt
#   possibly remove: 0 0.00 words
#   possibly signal variations/root word in the file so they can be suggested in order
#   make american, australian, austrian etc. start with caps

import re

MIN_COUNT = 40
IN_FILE_NAME = "ucrel_word_list.txt"
NGRAMS_FILE_NAME = "ngrams_top_50000.txt"
OUT_FILE_NAME = "parsed_word_list.txt"

acceptableWordRegex = re.compile('^[a-zA-Z][a-z-\']*[a-z]+$')
romanNumeralRegex = re.compile('xi|xl|xv|xx|vi')

addedWordCount = 0

addedWordsSet = set()

# set of two letter words
twoLetterWords = {'am', 'an', 'as', 'at', 'be', 'by', 'do', 'go', 'hi', 'if', 'in', 'is', 
        'it', 'my', 'no', 'of', 'oh', 'on', 'or', 'to', 'up', 'us', 'we', 'ya'}

# parts of speech of unclassified nouns. NoP variations are considered default and excluded
labelledNounVariations = {'Aunty':'NoC', 'Brother':'NoC', 'grassroot':'-',
        'jean':'NoC', 'Lady':'NoC', 'pincer':'NoC', 'pyjama':'NoC', 'scissor':'NoC',
        'Secretary':'NoC', 'stair':'NoP', 'suspender':'NoC', 'trouser':'NoC', 
        'tweezer':'NoC'}

# parts of speech of unclassifed verbs. VerbPa variations are considered default
labelledVerbVariations = {'doe':'-', 'wan':'-'}

excludeList = ('bureaux', 'phd', 'phds', 'beginned', 'affraid', 'cometh', 'doth', 
    'fraid', 'drived', 'gainsaid', 'giveth', 'gon', 'sitteth', 'rided', 'thingy', 'thingys', 
    'thingies', 'tian', 'tibi', 'haved', 'somethin', 'thee', 'thy', 'thyself', 'isbn', 
    'juste', 'justes', 'theirselves', 'yer', 'thou', 'utd', 'vdu', 'vis')

# the list of all word entries to print to file
wordList = list()

# add a word to wordList
def addWord(wordEntry):
    if (wordEntry[0] == 'a' and wordEntry[1] == "Det") or wordEntry[0] == 'I':
        print "GH"

    if wordEntry[0] in excludeList:
        return
    if len(wordEntry[0]) == 2 and not wordEntry[0] in twoLetterWords:
        return


    global addedWordCount
    addedWordCount += 1

    wordList.append(wordEntry)

# end addWord()


def addPronoun(wordEntry):
    if int(wordEntry[2]) < 10:
        return

    if (wordEntry[0] == 'i'):
        wordEntry = ('I', "Pron", wordEntry[2], wordEntry[3])
    else:
        wordEntry = (wordEntry[0], "Pron", wordEntry[2], wordEntry[3])

    addWord(wordEntry)

# end addPronoun()


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

# return True if a word should be filtered (ie. not added to the file)
def filterWord(lineList):
    # don't filter the Det word 'a' and proper noun 'I'
    if (lineList[0] == 'a' and lineList[1] == "Det") or (lineList[0] == 'i' 
            and lineList[1] == "Pron"):
        return False

    # filter using acceptable word regex
    if not acceptableWordRegex.match(lineList[0]):
        return True

    # filter roman numerals
    if lineList[1] == "Num" and romanNumeralRegex.match(lineList[0]):
        return True

    # filter unclassified and 'Fore' words
    if lineList[1] == "Uncl" or lineList[1] == "Fore":
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
        #return True

    # use different filtering for proper nouns
    if lineList[1] == "NoP":
        if frequency == 1 and dispersion >= 0.75:
            return False
        if frequency >= 2 and dispersion >= 0.58:
            return False
        if frequency >= 3 and dispersion >= 0.10:
            return False
        return True

        return True

    # if the word's frequency/dispersion combination is too low, skip it
    if frequency == 0 and dispersion < 0.84:
        return True
    elif frequency == 1 and dispersion < 0.77:
        return True
    elif frequency == 2 and dispersion < 0.62:
        return True
    elif frequency == 3 and dispersion < 0.62:
        return True
    elif frequency >= 4 and dispersion < 0.55:
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

       
def addVariation(lineList, originalPoS, originalWord):
    additionalEntries = list()
    wordEntry = [lineList[2], "", lineList[3], lineList[5]]

    if wordEntry[0] == 'i':
        addPronoun(wordEntry)
        return

    # if the variation is actually the original word, no extra processing is required
    if lineList[2] == originalWord:
        wordEntry[1] = originalPoS

        # if the PoS is a proper noun, add the possessive and plural variations
        if wordEntry[1] == "NoP" and not wordEntry[0] == 'I':
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
            if wordEntry[0] in labelledNounVariations:
                if labelledNounVariations[wordEntry[0]] == '-':
                    return
                wordEntry[1] = labelledNounVariations[wordEntry[0]]
            else:
                wordEntry[1] = "NoCPl"

    elif (originalPoS == "Verb"):
        if wordEntry[0][-3:]  == "ing":     # gerund verb
            wordEntry[1] = "VerbGe"
        elif wordEntry[0][-2:] == "ed" or wordEntry[0][-2:] == "en":    # past tense
            wordEntry[1] = "VerbPa"
        elif wordEntry[0][-1:] == "s":     # verb paired with thid-person singluar noun
            wordEntry[1] = "VerbTPP"
        else:
            if wordEntry[0] in labelledVerbVariations:
                if labelledVerbVariations[wordEntry[0]] == '-':
                    return
                wordEntry[1] = labelledVerbVariations[wordEntry[0]]
            else:
                wordEntry[1] = "VerbPa"

    elif originalPoS == "Adj":
        wordEntry[1] = "Adj"

    elif originalPoS == "Num":
        wordEntry[1] = "Num"
    
    elif originalPoS == "Pron":
        addPronoun(wordEntry)
        return

    else:
        # mostly NoPPl (Einsteins, Romas) here, these are excluded and generated manually
        # to ensure all proper nouns have plural variations included
        return

    addWord(wordEntry)

    for additionalEntry in additionalEntries:
        addWord(additionalEntry)
    

# end addVariation()

def addValidWords():
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

    # iterates and checks all lines that have a word in the first column
    for line in inFile:
        lineList = line.split()

        # if the line is that of a variation, handle a variation instead of a normal line
        if lineList[0] == '@':
            # if this variation isn't to be included, skip to the next line
            if includingVariations and (acceptableWordRegex.match(lineList[2]) or 
                    lineList[2] == 'a' or lineList[2] == 'i'):
                addVariation(lineList, originalPoS, originalWord)
            continue
        
        # check if the word should be filtered (not included)
        if (filterWord(lineList)):
            includingVariations = False
            continue

        if lineList[0] == 'a' and lineList[1] == "Det":
            #includingVariations = False
            #wordEntry = (lineList[0], lineList[1], lineList[3], lineList[5])
            print "a"
            
        # avoid adding variations for the word word 'I'
        if lineList[0] == 'i' and lineList[1] == "Pron":
            #includingVariations = False
            #wordEntry = (lineList[0], lineList[1], lineList[3], lineList[5])
            #addPronoun(wordEntry)
            print "I"

        # if the word has variations, prepare to include its variations then continue
        if lineList[2] == "%":
            includingVariations = True
            originalPoS = lineList[1]
            originalWord = lineList[0]
            continue

        # handle the "Ex" word listing for there
        if lineList[0] == "there" and lineList[1] == "Ex":
            wordEntry = (lineList[0], "Pron", lineList[3], lineList[5])
            addWord(wordEntry)

            wordEntry = (lineList[0], "NoC", lineList[3], lineList[5])
            addWord(wordEntry)

            wordEntry = (lineList[0], "Int", lineList[3], lineList[5])
            addWord(wordEntry)

            wordEntry = (lineList[0], "Adj", lineList[3], lineList[5])
            addWord(wordEntry)

            continue


        
        # no other conditions have been met so we should be fine to just add the line's data
        # to the word entry
        wordEntry = (lineList[0], lineList[1], lineList[3], lineList[5])

        # use special function to handle pronouns
        if wordEntry[1] == "Pron":
            addPronoun(wordEntry)
            continue

        addWord(wordEntry)

        additionalEntries = list()

        # if the PoS is a proper noun, add the possessive and plural variations
        if wordEntry[1] == "NoP" and not wordEntry[0] == 'I':
            # create artificial frequency and distribution for proper noun variations
            additionalEntries.append(getPossessiveEntry(wordEntry))
            additionalEntries.append(getPluralEntry(wordEntry))

        # if PoS is a common noun, add possessive variation
        #if wordEntry[1] == "NoC":
            #additionalEntries.append(getPossessiveEntry(wordEntry))

        for additionalEntry in additionalEntries:
            addWord(additionalEntry)

# end addValidWords()



addValidWords()

# print the first WORDS_REQUIRED words to file
print "Printing to file: " + str(OUT_FILE_NAME)

outFile = open(OUT_FILE_NAME, 'w')

for wordEntry in wordList:
    outFile.write(wordEntry[0] + '\t' + wordEntry[1] + '\t' + wordEntry[2] + '\t' + 
            wordEntry[3] + '\n')


print "Word count: " + str(addedWordCount)
