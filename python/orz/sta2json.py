#!/usr/bin/env python
# coding: UTF-8

import os
import struct
from sta import *
import json
import copy


class Stream:
    def __init__(self, byte):
        self.byte = byte
        self.index = 0

    def read(self, size=None):
        data = ''
        if size is None:
            data = self.byte[self.index:]
        else:
            data = self.byte[self.index:self.index+size]
        self.index += len(data)
        return data


def unpack_nil(stream, **params):
    stream.read(1)
    return None


def unpack_int(stream, **params):
    return struct.unpack('=i', stream.read(4))[0]


def unpack_float(stream, **params):
    return struct.unpack('=f', stream.read(4))[0]


def unpack_string(stream, **params):
    length = struct.unpack('=i', stream.read(4))[0]
    s = struct.unpack('=%ds' % length, stream.read(length))[0]
    return s


def unpack_binary(stream, **params):
    length = struct.unpack('=i', stream.read(4))[0]
    s = struct.unpack('=%dp' % length, stream.read(length))[0]

    mode = 1
    if 'binary_mode' in params:
        mode = params['binary_mode']

    if mode == 0:
        return '@binary@%d' % length
    elif mode == 1:
        # save file
        if 'getway' not in params:
            raise Exception("getway must be set.")
        if 'workshop' not in params:
            raise Exception("workshop must be set.")
        filename_ext = params['getway'] + '.bin'
        binary_filename = os.path.join(params['workshop'], filename_ext)
        with open(binary_filename, 'wb') as f:
            f.write(s)
        return '@file@%s' % filename_ext
    else:
        return s


def unpack_list(stream, **params):
    local_params = copy.copy(params)
    if 'getway' not in local_params:
        local_params['getway'] = ''
    getway = local_params['getway']

    obj = []
    length = struct.unpack('=i', stream.read(4))[0]
    for i in xrange(length):
        local_params['getway'] = getway + '_' + str(i)
        obj.append(unpack_obj(stream, **local_params))
    return obj


def unpack_dict(stream, **params):
    local_params = copy.copy(params)
    if 'getway' not in local_params:
        local_params['getway'] = ''
    getway = local_params['getway']

    obj = {}
    length = struct.unpack('=i', stream.read(4))[0]
    for i in xrange(length):
        key = unpack_string(stream, **params)
        local_params['getway'] = getway + '_' + key
        value = unpack_obj(stream, **local_params)
        obj[key] = value
    return obj


def unpack_obj(stream, **params):
    """
    unpack_obj
    :param stream: Stream of binary sta file
    :param params:
    :return:
    params['workshop']: means path to write binary file
    params['getway']: means the getway to all values
    params['binary_model']: 0(default): means write @binary@size
                            1: means @file@path
                            2: meas str for binary memory
    """
    mark = struct.unpack('=b', stream.read(1))[0]
    if mark == STA_NIL:
        return unpack_nil(stream, **params)
    elif mark == STA_INT:
        return unpack_int(stream, **params)
    elif mark == STA_FLOAT:
        return unpack_float(stream, **params)
    elif mark == STA_STRING:
        return unpack_string(stream, **params)
    elif mark == STA_BINARY:
        return unpack_binary(stream, **params)
    elif mark == STA_LIST:
        return unpack_list(stream, **params)
    elif mark == STA_DICT:
        return unpack_dict(stream, **params)
    else:
        raise Exception("Unsupported mark type: ", type(mark))


def sta2obj(sta_filename):
    byte = ''
    with open(sta_filename, 'rb') as ifile:
        byte = ifile.read()

    stream = Stream(byte)

    mark = struct.unpack('=i', stream.read(4))[0]

    if mark != 0x19910505:
        raise Exception("%s is not a valid sta file." % sta_filename)

    params = {}
    params['binary_mode'] = 0

    obj = unpack_obj(stream, **params)

    return obj


def sta2json(sta_filename, json_filename=None, **params):
    """
    sta2json
    :param sta_filename: input sta filename
    :param json_filename: output json filename or path
    :param params:
    :return:
    params['binary_model']: 0(default): means write @binary@size
                            1: means @file@path
                            2: meas str for binary memory
    """
    filepath, filename_ext = os.path.split(sta_filename)
    filename, ext = os.path.splitext(filename_ext)

    if json_filename is None:
        json_filename = os.path.join(filepath, filename + ".json")

    if os.path.isdir(json_filename):
        json_filename = os.path.join(json_filename, filename + ".json")

    workshop, getway_ext = os.path.split(json_filename)
    getway = os.path.splitext(getway_ext)[0]

    if len(workshop) > 0 and not os.path.isdir(workshop):
        raise Exception("%s/ is not a valid path." % workshop)

    with open(json_filename, 'w') as ofile:
        byte = ''
        with open(sta_filename, 'rb') as ifile:
            byte = ifile.read()

        stream = Stream(byte)

        mark = struct.unpack('=i', stream.read(4))[0]

        if mark != 0x19910505:
            raise Exception("%s is not a valid sta file." % sta_filename)

        params['workshop'] = workshop
        params['getway'] = getway

        obj = unpack_obj(stream, **params)

        json.dump(obj, ofile)
