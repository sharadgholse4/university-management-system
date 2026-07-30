#pragma once
#include "crow.h"
#include "reports/services/report_service.hpp"
#include "auth/services/jwt_service.hpp"

namespace ums {

class ReportController {
public:
    ReportController(ReportService& reportService, JwtService& jwtService);
    void registerRoutes(crow::SimpleApp& app);

private:
    ReportService& reportService_;
    JwtService& jwtService_;
};

} // namespace ums
