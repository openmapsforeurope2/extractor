#ifndef _APP_PARAMS_THEMEPARAMETERS_H_
#define _APP_PARAMS_THEMEPARAMETERS_H_

//STL
#include <string>

//EPG
#include <epg/params/ParametersT.h>
#include <epg/SingletonT.h>



	enum THEME_PARAMETERS{
		DB_CONF_FILE,
		COUNTRY_CODE_W,
		REF_SCHEMA,
		UP_SCHEMA,
		WK_SCHEMA,
		THEME_W,
		TABLE_W,
		TABLE_REF_SUFFIX,
		TABLE_UP_SUFFIX,
		TABLE_WK_SUFFIX,
		TABLE_WK_IDS_SUFFIX,
		TABLE_UP_AREA_SUFFIX,

		EX_BBOX_SIDE_THRESHOLD
	};

namespace app{
namespace params{

	class ThemeParameters : public epg::params::ParametersT< THEME_PARAMETERS >
	{
		typedef  epg::params::ParametersT< THEME_PARAMETERS > Base;

		public:

			/// \brief
			ThemeParameters();

			/// \brief
			~ThemeParameters();

			/// \brief
			virtual std::string getClassName()const;

	};

	typedef epg::Singleton< ThemeParameters >   ThemeParametersS;

}
}

#endif