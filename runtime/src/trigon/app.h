#pragma once

class app_t {
public:




private:

	 app_t();
	~app_t();

public: //singleton

	app_t(const app_t&) = delete;
	app_t& operator=(const app_t&) = delete;
	static inline app_t& get() {
		static app_t instance;
		return instance;
	}

};