
#include "Python.h"

#include <net-snmp/net-snmp-config.h>

#include <net-snmp/config_api.h>
#include <net-snmp/library/snmp_logging.h>
#include <net-snmp/library/tools.h>
#include <net-snmp/mib_api.h>

static int initialized = 0;

u_char *
oid_to_name(char *objid, int output_type)
{
    u_char *buf = NULL;
    size_t  objidlen;
    size_t  buf_len = 256;
    size_t  out_len = 0;
    int     buf_overflow = 0;
    oid     name[MAX_OID_LEN];

    netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, output_type);

    objidlen = MAX_OID_LEN;
    if (!read_objid(objid, name, &objidlen)) {
        snmp_perror(objid);
        exit(2);
    }

    buf = (u_char *) calloc(buf_len, 1);
    netsnmp_sprint_realloc_objid_tree(&buf, &buf_len, &out_len, 1,
                                      &buf_overflow, name, objidlen);

    return buf;
}


static PyObject *
oid_translate_init(PyObject *self, PyObject *args)
{
    int priority = LOG_DEBUG;
    int pri_max  = LOG_EMERG;
    netsnmp_log_handler *logh;

    shutdown_snmp_logging();
    logh = netsnmp_register_loghandler(NETSNMP_LOGHANDLER_NONE, priority);
    if (logh) logh->pri_max = pri_max;

    init_snmp("oid_translate");
    initialized = 1;
    Py_RETURN_NONE;
}


static PyObject *
oid_translate_translate(PyObject *self, PyObject *args)
{
    char *oid;
    u_char     *output;
    int         output_type;
    PyObject   *out;

    if (!PyArg_ParseTuple(args, "si", &oid, &output_type))
        return NULL;

    if (!initialized){
        netsnmp_init_mib();
        initialized = 1;
    }

    output = oid_to_name(oid, output_type);
    out = Py_BuildValue("s", output);
    SNMP_FREE(output);
    return out;
}


static PyMethodDef OidTranslateMethods[] = {
    {"init",  oid_translate_init, METH_VARARGS, "Initialize MIB data."},
    {"translate",  oid_translate_translate, METH_VARARGS, "Translate OID to Name or vice versa."},
    {NULL, NULL, 0, NULL},
};


PyMODINIT_FUNC
init_oid_translate(void)
{
    (void) Py_InitModule("_oid_translate", OidTranslateMethods);
}
