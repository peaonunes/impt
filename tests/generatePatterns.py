import os
import random

fileName = '../data/proteins.100MB'
size = os.path.getsize(fileName)

f = open(fileName, 'r')
text = f.read()

patterns = []

'''
for i in range(1, 102):
	end = random.randint(1, size)
	start = end-100

	if (start < 0):
		start = end+100

	sub = text[start:end]
	pat = sub[:i]
	patterns = patterns + [pat]
'''

for i in range(100,5000,500):
	print(i)
	start = random.randint(1,size)
	end = random.randint(start,size)

	#while(start-end < 0):
	#	start = random.randint(1,size)

	sub = text[start:end]
	pat = sub[:i]

	patterns = patterns + [pat]



text_file = open("HugePatterns.txt", "w")
for i in patterns:
	text_file.write("patterns.push_back(\"{0}\");\n".format(i))
text_file.close()