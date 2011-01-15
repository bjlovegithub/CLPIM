#! /usr/bin/python

blockSize = 8192
fileName = "GetNodeTest.in"
fh = open(fileName, "wb");
keysDelimiter = "\02\02\02\02"
keyValueDelimiter = "\01\01\01\01"
dummyChar = '\0'

#Write header info
nodeSpaceUsed = 0
fh.write("0.0.0")
nodeSpaceUsed += 5
fh.write(keysDelimiter)
#write keyNum/maxkeyNum/root offset
fh.write("\0\0\0\0\0\0\0\0")
fh.write("\7\7\7\7\7\7\0\0")
fh.write("\1\0\0\0\0\0\0\0")
nodeSpaceUsed += 28

for i in range(nodeSpaceUsed, blockSize):
    fh.write(dummyChar)

#Write the first node info
nodeSpaceUsed = 0
# 3 elements in this node
fh.write("\3\0\0\0")
nodeSpaceUsed += 4
# it is a leaf node
fh.write("\1\0\0\0")
nodeSpaceUsed += 4
# element 1
fh.write("\4\0\0\0")
fh.write("key3")
nodeSpaceUsed += 8
fh.write("\157\0\0\0\0\0\0\0") # TODO - consider about endian on other platform
nodeSpaceUsed += 8
# element 2
fh.write("\4\0\0\0")
fh.write("key1")
nodeSpaceUsed += 8
fh.write("\160\0\0\0\0\0\0\0")
nodeSpaceUsed += 8
# element 3
fh.write("\4\0\0\0")
fh.write("key2")
nodeSpaceUsed += 8
fh.write("\161\0\0\0\0\0\0\0")
nodeSpaceUsed += 8
# add the last pointer
fh.write("\0\0\0\0\0\0\0\0");
nodeSpaceUsed += 8
# fill the left space
print "Space Used: %d" % nodeSpaceUsed
for i in range(nodeSpaceUsed, blockSize):
    fh.write(dummyChar)

fh.close()
