def increment (num):
    return num + 1

def mergeIn (num):
    newnum = ""
    strnum = str(num)
    for i in range(len(strnum) - 2):
        newnum += strnum[i]
    newnum += str(int(strnum[-1:]) + int(strnum[-2:-1]))
    return int(newnum)

def mergeOne (num):
    return (num*10)+1

def getDigSum (num):
    temp = 0
    for i in str(num):
        temp += int(i)
    return temp

def discoverTerms(startnum, target):
    if getDigSum(startnum) < target:
        print("{" + str(startnum) + "}")
        discoverTerms(mergeOne(startnum), target)
        try:
            print("}<-")
            discoverTerms(mergeIn(startnum), target)
        except:
            print("this is an end probably!")
    elif getDigSum(startnum) == target:
        print(startnum) #make this add to a list with all valid nums in it
        print(str(mergeIn(startnum)) + "<-")
        termls.append(startnum)
        termls.append(mergeIn(startnum))
    else:
        print("What?")

numtogetto = 5
a = 1
termls = []
discoverTerms(a, numtogetto)
print(termls)
