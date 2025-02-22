#ifndef _APP_CALCUL_UPAREAEXTRACTIONOP_H_
#define _APP_CALCUL_UPAREAEXTRACTIONOP_H_


//SOCLE
#include <ign/feature/sql/FeatureStorePostgis.h>

//EPG
#include <epg/log/EpgLogger.h>
#include <epg/log/ShapeLogger.h>


namespace app{
namespace calcul{

	class UpAreaExtractionOp {

	public:

	
        /// @brief 
        /// @param countryCode 
        /// @param verbose 
        UpAreaExtractionOp(
            std::string countryCode,
            bool verbose
        );

        /// @brief 
        ~UpAreaExtractionOp();


		/// \brief
		static void Compute(
			std::string countryCode, 
			bool verbose
		);


	private:
		//--
		ign::feature::sql::FeatureStorePostgis*                  _fsSource;
		//--
		ign::feature::sql::FeatureStorePostgis*                  _fsTarget;
		//--
		// ign::feature::sql::FeatureStorePostgis*                  _fsRef;
		// //--
		// ign::feature::sql::FeatureStorePostgis*                  _fsUp;
		// //--
		// ign::feature::sql::FeatureStorePostgis*                  _fsCd;
		//--
		ign::feature::sql::FeatureStorePostgis*                  _fsBorder;
		//--
		ign::feature::sql::FeatureStorePostgis*                  _fsUpArea;
		//--
		epg::log::EpgLogger*                                     _logger;
		//--
		epg::log::ShapeLogger*                                   _shapeLogger;
		//--
		std::string                                              _countryCode;
		//--
		bool                                                     _verbose;

	private:

		//--
		void _init();

		//--
		void _compute() const;

    };

}
}

#endif