import _oid_translate

OID_OUTPUT_DEFAULT = 0
OID_OUTPUT_SUFFIX = 1
OID_OUTPUT_MODULE = 2
OID_OUTPUT_FULL = 3
OID_OUTPUT_NUMERIC = 4
OID_OUTPUT_UCD = 5
OID_OUTPUT_NONE = 6


def load_mibs():
    """ Preload MIB files into tree.

    This isn't necessary, as MIBs will be loaded lazily the first time
    a translation is requested. This will preload the MIB files ahead
    of time.
    """
    _oid_translate.init()


def translate(oid, output_type=OID_OUTPUT_DEFAULT):
    return _oid_translate.translate(oid, output_type)


def get_details(oid):
    return _oid_translate.get_details(oid)


class ObjectId(object):
    def __init__(self, objid):
        # Do a translation now to find bad values early.
        self.oid = translate(objid, OID_OUTPUT_NUMERIC)
        self._details = None

    @property
    def name(self):
        return translate(self.oid, OID_OUTPUT_DEFAULT)

    @property
    def suffix(self):
        return translate(self.oid, OID_OUTPUT_SUFFIX)

    @property
    def fullname(self):
        return translate(self.oid, OID_OUTPUT_FULL)

    @property
    def hint(self):
        if self._details is None:
            self._details = get_details(self.oid)
        return self._details.get("hint")

    @property
    def enums(self):
        if self._details is None:
            self._details = get_details(self.oid)
        return self._details.get("enums", {})

    @property
    def textual(self):
        if self._details is None:
            self._details = get_details(self.oid)
        return self._details.get("textual")
