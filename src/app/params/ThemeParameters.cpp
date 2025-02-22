
//APP
#include <app/params/ThemeParameters.h>

//SOCLE
#include <ign/Exception.h>


namespace app{
namespace params{


	///
	///
	///
	ThemeParameters::ThemeParameters()
	{
		_initParameter( DB_CONF_FILE, "DB_CONF_FILE");
		_initParameter( COUNTRY_CODE_W, "COUNTRY_CODE_W");
		_initParameter( REF_SCHEMA, "REF_SCHEMA");
		_initParameter( UP_SCHEMA, "UP_SCHEMA");
		_initParameter( WK_SCHEMA, "WK_SCHEMA");
		_initParameter( THEME_W, "THEME_W");
		_initParameter( TABLE_W, "TABLE_W");
		_initParameter( TABLE_REF_SUFFIX, "TABLE_REF_SUFFIX");
		_initParameter( TABLE_UP_SUFFIX, "TABLE_UP_SUFFIX");
		_initParameter( TABLE_WK_SUFFIX, "TABLE_WK_SUFFIX");
		_initParameter( TABLE_WK_IDS_SUFFIX, "TABLE_WK_IDS_SUFFIX");
		_initParameter( TABLE_UP_AREA_SUFFIX, "TABLE_UP_AREA_SUFFIX");


		_initParameter( EX_BBOX_SIDE_THRESHOLD, "EX_BBOX_SIDE_THRESHOLD");
	}

	///
	///
	///
	ThemeParameters::~ThemeParameters()
	{
	}

	///
	///
	///
	std::string ThemeParameters::getClassName()const
	{
		return "app::params::ThemeParameters";
	}


}
}