import random
consonants = ['m','ng','r','l','t','s','k','d','b','h','-']
vowels = ['a','i','u','o']
for i in range(30):
    if random.randint(0,1) == 0:
        print(consonants[random.randint(0,10)] + vowels[random.randint(0,3)], end=' ')
    else:
        print(consonants[random.randint(0,10)] + vowels[random.randint(0,3)] + consonants[random.randint(0,3)], end=' ')
