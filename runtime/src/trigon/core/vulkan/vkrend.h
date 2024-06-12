#pragma once
#ifdef _RUNTIME

class vkrend_t {
private:

	vkrend_t();
	~vkrend_t();





public: //singleton
	vkrend_t(const vkrend_t&) = delete;
	vkrend_t& operator=(const vkrend_t&) = delete;
	static inline vkrend_t& get() {
		static vkrend_t instance;
		return instance;
	}
};

#endif