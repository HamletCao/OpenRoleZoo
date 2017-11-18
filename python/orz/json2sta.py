#!/usr/bin/env python
# coding: utf-8

import json
import os
from sta import *
import struct
import time


_registered_command = {}


def register_command(cmd, callback):
    _registered_command[cmd] = callback


def pack_command(cmd, **params):
    if len(cmd) == 0 or cmd[0] != '@':
        return pack_string(cmd)
    key_params = cmd[1:].split('@')
    key = key_params[0]
    cmd_params = key_params[1:]
    if key in _registered_command:
        return _registered_command[key](*cmd_params, **params)
    return pack_string(cmd, **params)


def pack_nil(**params):
    byte = struct.pack('=bb', STA_NIL, 0)
    return byte


def pack_int(obj, **params):
    byte = struct.pack('=bi', STA_INT, int(obj))
    return byte


def pack_float(obj, **params):
    byte = struct.pack('=bf', STA_FLOAT, float(obj))
    return byte


def pack_raw_string(obj, **params):
    s = str(obj)
    byte = struct.pack('=i%ds' % len(s), len(s), s)
    return byte


def pack_string(obj, **params):
    s = str(obj)
    byte = struct.pack('=bi%ds' % len(s), STA_STRING, len(s), s)
    return byte


def pack_binary(obj, **params):
    s = str(obj)
    byte = struct.pack('=bi%dp' % len(s), STA_BINARY, len(s), s)
    return byte


def pack_list(obj, **params):
    byte = struct.pack('=bi', STA_LIST, len(obj))
    for value in obj:
        byte += pack_obj(value, **params)
    return byte


def pack_dict(obj, **params):
    byte = struct.pack('=bi', STA_DICT, len(obj))
    for key in obj.keys():
        value = obj[key]
        byte += pack_raw_string(key, **params)
        byte += pack_obj(value, **params)
    return byte


def pack_value(value, **params):
    if value is None:
        return pack_nil(**params)
    elif isinstance(value, unicode):
        return pack_value(value.encode("UTF-8"), **params)
    elif isinstance(value, str):
        if len(value) > 0 and value[0] == '@':
            return pack_command(value, **params)
        return pack_string(value, **params)
    elif isinstance(value, int):
        return pack_int(value, **params)
    elif isinstance(value, float):
        return pack_float(value, **params)
    elif isinstance(value, binary):
        return pack_binary(value, **params)
    else:
        raise Exception("Unsupported value type: ", type(value))


def pack_obj(obj, **params):
    if isinstance(obj, dict):
        return pack_dict(obj, **params)
    elif isinstance(obj, list):
        return pack_list(obj, **params)
    else:
        return pack_value(obj, **params)


def pack_date(**params):
    date = time.strftime("%Y-%m-%d", time.localtime())
    return pack_string(date, **params)


def pack_time(**params):
    date = time.strftime("%H:%M:%S", time.localtime())
    return pack_string(date, **params)


def pack_datetime(**params):
    date = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    return pack_string(date, **params)


def pack_file(filename, **params):
    if 'workshop' in params:
        filename = os.path.join(params['workshop'], filename)
    if not os.path.isfile(filename):
        raise Exception("%s is not a valid file." % filename)

    data = ''
    with open(filename, 'rb') as f:
        data = f.read()

    return pack_binary(data, **params)


register_command("date", pack_date)
register_command("time", pack_time)
register_command("datetime", pack_datetime)
register_command("file", pack_file)


def obj2sta(obj, sta_filename, workshop = None):
    if workshop is None:
        workshop = ''

    with open(sta_filename, 'wb') as ofile:

        params = {}
        params['workshop'] = workshop

        byte = pack_obj(obj, **params)

        # write header
        ofile.write(struct.pack('i', 0x19910505))
        # write content
        ofile.write(byte)


def json2sta(json_filename, sta_filename=None):
    filepath, filename_ext = os.path.split(json_filename)
    filename, ext = os.path.splitext(filename_ext)

    if sta_filename is None:
        sta_filename = os.path.join(filepath, filename + ".sta")

    if os.path.isdir(sta_filename):
        sta_filename = os.path.join(sta_filename, filename + ".sta")

    output_path = os.path.split(sta_filename)[0]
    workshop = filepath

    if len(output_path) > 0 and not os.path.isdir(output_path):
        raise Exception("%s/ is not a valid path." % output_path)

    with open(sta_filename, 'wb') as ofile:
        obj = {}
        with open(json_filename) as ifile:
            obj = json.load(ifile)

        params = {}
        params['workshop'] = workshop

        byte = pack_obj(obj, **params)

        # write header
        ofile.write(struct.pack('i', 0x19910505))
        # write content
        ofile.write(byte)
