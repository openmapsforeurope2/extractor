// APP
#include <app/calcul/UpAreaExtractionOp.h>
#include <app/params/ThemeParameters.h>

// BOOST
#include <boost/progress.hpp>

// EPG
#include <epg/Context.h>
#include <epg/params/EpgParameters.h>
#include <epg/sql/tools/numFeatures.h>
#include <epg/sql/DataBaseManager.h>
#include <epg/tools/StringTools.h>
#include <epg/tools/TimeTools.h>
#include <epg/tools/FilterTools.h>
#include <epg/tools/geometry/PolygonSplitter.h>


namespace app
{
    namespace calcul
    {
        ///
        ///
        ///
        UpAreaExtractionOp::UpAreaExtractionOp(
            std::string countryCode,
            bool verbose
        ) : 
            _countryCode(countryCode),
            _verbose(verbose)
        {
            _init();
        }

        ///
        ///
        ///
        UpAreaExtractionOp::~UpAreaExtractionOp()
        {

            // _shapeLogger->closeShape("ps_cutting_ls");
        }

        ///
        ///
        ///
        void UpAreaExtractionOp::Compute(
			std::string countryCode, 
			bool verbose
		) {
            UpAreaExtractionOp op(countryCode, verbose);
            op._compute();
        }

        ///
        ///
        ///
        void UpAreaExtractionOp::_init()
        {
            //--
            _logger = epg::log::EpgLoggerS::getInstance();
            _logger->log(epg::log::INFO, "[START] initialization: " + epg::tools::TimeTools::getTime());

            //--
            _shapeLogger = epg::log::ShapeLoggerS::getInstance();
            // _shapeLogger->addShape("ps_cutting_ls", epg::log::ShapeLogger::POLYGON);

            //--
            epg::Context *context = epg::ContextS::getInstance();

            // epg parameters
            epg::params::EpgParameters const& epgParams = context->getEpgParameters();
            std::string const idName = epgParams.getValue(ID).toString();
            std::string const geomName = epgParams.getValue(GEOM).toString();
            std::string const countryCodeName = epgParams.getValue(COUNTRY_CODE).toString();
            std::string const borderTableName = epgParams.getValue(TARGET_BOUNDARY_TABLE).toString();
            
            // app parameters
            params::ThemeParameters *themeParameters = params::ThemeParametersS::getInstance();;
            std::string const sourceTableName = themeParameters->getValue(TABLE_W).toString();
            std::string upAreaTableName = sourceTableName + themeParameters->getValue(TABLE_UP_AREA_SUFFIX).toString();
            std::string targetTableName = sourceTableName + themeParameters->getValue(TABLE_WK_SUFFIX).toString();
            std::string targetIdsTableName = sourceTableName + themeParameters->getValue(TABLE_WK_IDS_SUFFIX).toString();

            {
                std::ostringstream ss;
                ss << "DELETE FROM " << targetTableName << " ;";
                context->getDataBaseManager().getConnection()->update(ss.str());
            }

            {
                std::ostringstream ss;
                ss << "DELETE FROM " << targetIdsTableName << " ;";
                context->getDataBaseManager().getConnection()->update(ss.str());
            }

            //--
            _fsSource = context->getDataBaseManager().getFeatureStore(sourceTableName, idName, geomName);
            _fsTarget = context->getDataBaseManager().getFeatureStore(targetTableName, idName, geomName);
            // _fsBorder = context->getDataBaseManager().getFeatureStore(borderTableName, idName, geomName);
            _fsUpArea = context->getDataBaseManager().getFeatureStore(upAreaTableName, idName, geomName);

            //--
            _logger->log(epg::log::INFO, "[END] initialization: " + epg::tools::TimeTools::getTime());
        };

        ///
        ///
        ///
        void UpAreaExtractionOp::_compute() const {
            //--
            epg::Context *context = epg::ContextS::getInstance();

            // epg parameters
            epg::params::EpgParameters const& epgParams = context->getEpgParameters();
            std::string const idName = epgParams.getValue(ID).toString();
            std::string const geomName = epgParams.getValue(GEOM).toString();
            std::string const countryCodeName = epgParams.getValue(COUNTRY_CODE).toString();

            // app parameters
            params::ThemeParameters *themeParameters = params::ThemeParametersS::getInstance();
            double const bboxThreshold = themeParameters->getValue(EX_BBOX_SIDE_THRESHOLD).toDouble();

            //--
            ign::feature::FeatureFilter areaFilter;

            int nAreas = epg::sql::tools::numFeatures(*_fsUpArea, areaFilter);
            boost::progress_display display(nAreas, std::cout, "[ extracting % complete ]\n");

            std::set<std::string> sExtracted;
            
            ign::feature::FeatureIteratorPtr itArea = _fsUpArea->getFeatures(areaFilter);
            while (itArea->hasNext())
            {
                ++display;

                ign::feature::Feature const& fArea = itArea->next();
                ign::geometry::MultiPolygon const& mp = fArea.getGeometry().asMultiPolygon();

                for (size_t i = 0 ; i < mp.numGeometries() ; ++i) {
                    ign::geometry::Polygon const& poly = mp.polygonN(i);
                    ign::geometry::Envelope bbox = poly.getEnvelope();

                    std::vector<ign::geometry::Polygon> vPoly;
                    if ( bbox.width() <= bboxThreshold && bbox.height() <= bboxThreshold ) {
                        vPoly.push_back(poly);
                    } else {
                        epg::tools::geometry::PolygonSplitter splitter(poly);

                        double currentX = bbox.xmin()+bboxThreshold;
                        while( currentX < bbox.xmax()) {
                            splitter.addCuttingGeometry(ign::geometry::LineString(ign::geometry::Point(currentX, bbox.ymin()), ign::geometry::Point(currentX, bbox.ymax())));
                            currentX += bboxThreshold;
                        }

                        double currentY = bbox.ymin()+bboxThreshold;
                        while( currentY < bbox.ymax()) {
                            splitter.addCuttingGeometry(ign::geometry::LineString(ign::geometry::Point(bbox.xmin(), currentY), ign::geometry::Point(bbox.xmax(), currentY)));
                            currentY += bboxThreshold;
                        }
                        

                        splitter.split(vPoly);
                    }

                    for (size_t j = 0 ; j < vPoly.size() ; ++j) {
                        ign::feature::FeatureFilter filter("ST_INTERSECTS("+geomName+", ST_SetSRID(ST_GeomFromText('"+ vPoly[j].toString() +"'),3035))");
                        ign::feature::FeatureIteratorPtr itFeat = _fsSource->getFeatures(filter);
                        while (itFeat->hasNext())
                        {
                            ign::feature::Feature feat = itFeat->next();
                            std::string const featId = feat.getId();

                            if (sExtracted.find(featId) != sExtracted.end()) continue;
                            sExtracted.insert(featId);

                            _fsTarget->createFeature(feat, featId);
                        }
                    }
                }
            }

            {
                std::string const sourceTableName = themeParameters->getValue(TABLE_W).toString();
                std::string targetTableName = sourceTableName + themeParameters->getValue(TABLE_WK_SUFFIX).toString();
                std::string targetIdsTableName = sourceTableName + themeParameters->getValue(TABLE_WK_IDS_SUFFIX).toString();

                std::ostringstream ss;
                ss << "INSERT INTO " << targetIdsTableName << " (" << idName << ") SELECT " << idName << " FROM " << targetTableName << ";";
                context->getDataBaseManager().getConnection()->update(ss.str());
            }
        }
    }
}
