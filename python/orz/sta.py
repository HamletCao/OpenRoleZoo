#!/usr/bin/env python
# coding: UTF-8

STA_NIL = 0
STA_INT = 1
STA_FLOAT = 2
STA_STRING = 3
STA_BINARY = 4
STA_LIST = 5
STA_DICT = 6


class binary(object):
    def __init__(self, byte=''):
        self.byte = byte

    def __str__(self):
        return str(self.byte)