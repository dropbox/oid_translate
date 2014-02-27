import _oid_translate

OID_OUTPUT_DEFAULT = 0
OID_OUTPUT_SUFFIX = 1
OID_OUTPUT_MODULE = 2
OID_OUTPUT_FULL = 3
OID_OUTPUT_NUMERIC = 4
OID_OUTPUT_UCD = 5
OID_OUTPUT_NONE = 6


def init_mibs():
    _oid_translate.init()


def translate(oid, output_type=OID_OUTPUT_DEFAULT):
    return _oid_translate.translate(oid, output_type)
