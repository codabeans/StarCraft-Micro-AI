#! /usr/bin/python

import sparcraft_pb2
import sys

# Iterates though all people in the Army and prints info about them.
def ListPeople(address_book):
  for unit in army.unit:
    print " ID:", unit.id
    print " HP:", unit.hp
    print " Pos X:", unit.posx
    print " Pos Y:", unit.posy

# Main procedure:  Reads the entire address book from a file and prints all
#   the information inside.
if len(sys.argv) != 2:
  print "Usage:", sys.argv[0], "ADDRESS_BOOK_FILE"
  sys.exit(-1)

army = sparcraft_pb2.Army()

# Read the existing address book.
f = open(sys.argv[1], "rb")
army.ParseFromString(f.read())
f.close()

ListPeople(army)
