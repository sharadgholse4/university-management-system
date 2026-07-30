#include "reports/controllers/report_controller.hpp"
#include "auth/middleware/auth_middleware.hpp"

namespace ums {

ReportController::ReportController(ReportService& reportService, JwtService& jwtService)
    : reportService_(reportService), jwtService_(jwtService) {}

void ReportController::registerRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/reports/department/<int>")
        .methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req, int departmentId) {
            auto ctxResult = authenticate(req, jwtService_);
            if (!isSuccess(ctxResult)) {
                return makeErrorResponse(getError(ctxResult));
            }

            const auto& ctx = getValue(ctxResult);
            // WHY: Only professors and admins can view department reports
            if (!authorize(ctx, Role::Professor)) {
                return makeErrorResponse(AppError{ErrorCode::Forbidden, "Insufficient permissions"});
            }

            auto result = reportService_.getDepartmentSummary(departmentId);
            if (!isSuccess(result)) {
                return makeErrorResponse(getError(result));
            }

            return makeResponse(200, successResponse(getValue(result).toJson()));
        });

    CROW_ROUTE(app, "/api/reports/course/<int>/attendance")
        .methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req, int courseId) {
            auto ctxResult = authenticate(req, jwtService_);
            if (!isSuccess(ctxResult)) {
                return makeErrorResponse(getError(ctxResult));
            }

            const auto& ctx = getValue(ctxResult);
            // WHY: Only professors and admins can view course attendance reports
            if (!authorize(ctx, Role::Professor)) {
                return makeErrorResponse(AppError{ErrorCode::Forbidden, "Insufficient permissions"});
            }

            auto result = reportService_.getCourseAttendanceReport(courseId);
            if (!isSuccess(result)) {
                return makeErrorResponse(getError(result));
            }

            crow::json::wvalue::list reportList;
            for (const auto& item : getValue(result)) {
                reportList.push_back(item.toJson());
            }

            return makeResponse(200, successResponse(crow::json::wvalue(std::move(reportList))));
        });

    CROW_ROUTE(app, "/api/reports/course/<int>/grades")
        .methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req, int courseId) {
            auto ctxResult = authenticate(req, jwtService_);
            if (!isSuccess(ctxResult)) {
                return makeErrorResponse(getError(ctxResult));
            }

            const auto& ctx = getValue(ctxResult);
            // WHY: Only professors and admins can view course grade reports
            if (!authorize(ctx, Role::Professor)) {
                return makeErrorResponse(AppError{ErrorCode::Forbidden, "Insufficient permissions"});
            }

            auto result = reportService_.getGradeDistributionReport(courseId);
            if (!isSuccess(result)) {
                return makeErrorResponse(getError(result));
            }

            return makeResponse(200, successResponse(getValue(result).toJson()));
        });

    CROW_ROUTE(app, "/api/reports/system")
        .methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) {
            auto ctxResult = authenticate(req, jwtService_);
            if (!isSuccess(ctxResult)) {
                return makeErrorResponse(getError(ctxResult));
            }

            const auto& ctx = getValue(ctxResult);
            // WHY: System overview contains sensitive overall numbers, only admins can view it
            if (!authorize(ctx, Role::Admin)) {
                return makeErrorResponse(AppError{ErrorCode::Forbidden, "Insufficient permissions"});
            }

            auto result = reportService_.getSystemOverview();
            if (!isSuccess(result)) {
                return makeErrorResponse(getError(result));
            }

            return makeResponse(200, successResponse(getValue(result).toJson()));
        });
}

} // namespace ums
