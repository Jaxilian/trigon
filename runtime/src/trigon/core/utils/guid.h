#ifndef TG_GUID_H
#define TG_GUID_H


#ifdef __cplusplus
extern "C" {
#endif
	

	typedef char guid_t[64];
	void guid_gen(guid_t id);

	typedef char serialkey_t[35];
	void serialkey_gen(serialkey_t id);
#ifdef __cplusplus
}
#endif


#endif