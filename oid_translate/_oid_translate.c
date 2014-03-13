
#include "Python.h"

#include <net-snmp/net-snmp-config.h>

#include <net-snmp/config_api.h>
#include <net-snmp/library/snmp_logging.h>
#include <net-snmp/library/tools.h>
#include <net-snmp/mib_api.h>

static int initialized = 0;


static void
_init_snmp(void)
{
    int priority = LOG_DEBUG;
    int pri_max  = LOG_EMERG;
    netsnmp_log_handler *logh;

    shutdown_snmp_logging();
    logh = netsnmp_register_loghandler(NETSNMP_LOGHANDLER_NONE, priority);
    if (logh) logh->pri_max = pri_max;

    init_snmp("oid_translate");
    initialized = 1;
}


static u_char *
objid_to_name(char *objid, int output_type)
{
    u_char *buf = NULL;
    size_t  buf_len = 256;
    size_t  out_len = 0;
    int     buf_overflow = 0;
    oid     oid[MAX_OID_LEN];
    size_t  objidlen = MAX_OID_LEN;

    if (!read_objid(objid, oid, &objidlen)) {
        snmp_perror(objid);
        return NULL;
    }

    netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, output_type);
    buf = (u_char *) calloc(buf_len, 1);
    netsnmp_sprint_realloc_objid_tree(&buf, &buf_len, &out_len, 1,
                                      &buf_overflow, oid, objidlen);

    return buf;
}


static PyObject *
oid_translate_init(PyObject *self, PyObject *args)
{
    _init_snmp();
    Py_RETURN_NONE;
}


static PyObject *
oid_translate_translate(PyObject *self, PyObject *args)
{
    char     *objid;
    int       output_type;

    u_char   *name;
    PyObject *out;

    if (!PyArg_ParseTuple(args, "si", &objid, &output_type))
        return NULL;

    if (!initialized) _init_snmp();

    name = objid_to_name(objid, output_type);
    if (!name){
        PyErr_SetString(PyExc_ValueError, snmp_api_errstring(snmp_errno));
        return NULL;
    }

    out = Py_BuildValue("s", name);
    SNMP_FREE(name);
    return out;
}


static PyObject *
oid_translate_get_details(PyObject *self, PyObject *args)
{
    char        *objid;
    struct tree *tp;
    oid          oid[MAX_OID_LEN];
    size_t       objidlen;

    if (!PyArg_ParseTuple(args, "s", &objid))
        return NULL;

    if (!initialized) _init_snmp();

    objidlen = MAX_OID_LEN;
    if (!read_objid(objid, oid, &objidlen)) {
        PyErr_SetString(PyExc_ValueError, snmp_api_errstring(snmp_errno));
        return NULL;
    }

    PyObject *outDict = PyDict_New();
    PyObject *enums = PyDict_New();

    tp = get_tree(oid, objidlen, get_tree_head());

    PyDict_SetItemString(outDict, "textual",
        Py_BuildValue("z", get_tc_descriptor(tp->tc_index))
    );
    PyDict_SetItemString(outDict, "hint",
        Py_BuildValue("z", tp->hint)
    );
    PyDict_SetItemString(outDict, "units",
        Py_BuildValue("z", tp->units)
    );

    if (tp->enums){
        struct enum_list *ep = tp->enums;
        while(ep){
            PyDict_SetItem(enums,
                Py_BuildValue("i", ep->value),
                Py_BuildValue("s", ep->label)
            );
            ep = ep->next;
        }
    }
    PyDict_SetItemString(outDict, "enums", enums);
    return outDict;
}


static PyMethodDef OidTranslateMethods[] = {
    {"init",  oid_translate_init, METH_VARARGS, "Initialize MIB data."},
    {"translate",  oid_translate_translate, METH_VARARGS, "Translate OID to Name or vice versa."},
    {"get_details",  oid_translate_get_details, METH_VARARGS, "Get extra details for OID."},
    {NULL, NULL, 0, NULL},
};


PyMODINIT_FUNC
init_oid_translate(void)
{
    (void) Py_InitModule("_oid_translate", OidTranslateMethods);
}
