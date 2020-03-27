/* ruby_tclink.c - Library code for the TCLink client API. */
 

#include <ruby.h>
#include "tclink.h"

static VALUE
tclink_getversion(VALUE obj) {
	return rb_str_new2(TCLINK_VERSION);
}

static VALUE
tclink_send(VALUE obj, VALUE params) {
	TCLinkHandle handle;
	char buf[4096];
	VALUE input_keys, input_key, input_value, result;
	char *result_key, *result_value, *next_result_key;
	int input_keys_count;
	int i = 0;

	handle = TCLinkCreate();

	/* grab the Ruby hash and stuff each parameter set into TCLink */
	input_keys = rb_funcall(params, rb_intern("keys"), 0, 0);
	input_keys_count = FIX2INT(rb_funcall(input_keys, rb_intern("length"),
                                   0, 0));

	for (i = 0; i < input_keys_count; i++) {
		input_key = rb_funcall(input_keys, rb_intern("[]"), 1,
                                       INT2FIX(i));
		input_value = rb_hash_aref(params, input_key);
		TCLinkPushParam(handle, RSTRING_PTR(StringValue(input_key)),
                                RSTRING_PTR(StringValue(input_value)));
	}

	/* send the transaction */
	TCLinkSend(handle);

	/* pull out the returned parameters and put them in a Ruby hash */
	TCLinkGetEntireResponse(handle, buf, sizeof(buf));

	result = rb_hash_new();
	result_key = result_value = buf;
	while (result_key && (result_value = strchr(result_key, '='))) {
		*result_value++ = 0;
		next_result_key = strchr(result_value, '\n');
		if (next_result_key) *next_result_key++ = 0;
		rb_hash_aset(result, rb_str_new2(result_key),
                             rb_str_new2(result_value));
		result_key = next_result_key;
	}

	TCLinkDestroy(handle);
	
	/* return the results hash */
	return result;
}

void
Init_tclink() {
	VALUE tclink = rb_define_module("TCLink");

	rb_define_module_function(tclink, "getVersion", tclink_getversion, 0);
	rb_define_module_function(tclink, "send", tclink_send, 1);
}
