/*
  Copyright (C) 2014-2017 Sven Willner <sven.willner@pik-potsdam.de>
                          Christian Otto <christian.otto@pik-potsdam.de>

  This file is part of Acclimate.

  Acclimate is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  Acclimate is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with Acclimate.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ACCLIMATE_SECTOR_H
#define ACCLIMATE_SECTOR_H

#include "acclimate.h"

namespace acclimate {

template<class ModelVariant>
class Model;
template<class ModelVariant>
class Firm;

template<class ModelVariant>
class Sector {
  protected:
    const std::string name;
    const IntType index_;
    Demand total_demand_D_ = Demand(0.0);
    Flow total_production_X_ = Flow(0.0);
    Flow last_total_production_X_ = Flow(0.0);
    typename ModelVariant::SectorParameters parameters_;

  public:
    inline const Demand& total_demand_D() const {
        assertstepnot(PURCHASE);
        return total_demand_D_;
    };
    inline const Demand& last_total_production_X() const { return last_total_production_X_; };
    inline const Demand& total_production_X() const {
        assertstepnot(CONSUMPTION_AND_PRODUCTION);
        return total_production_X_;
    };
    inline const typename ModelVariant::SectorParameters& parameters() const { return parameters_; };
    inline typename ModelVariant::SectorParameters& parameters_writable() {
        assertstep(INITIALIZATION);
        return parameters_;
    };

  public:
    const Ratio upper_storage_limit_omega;
    const Time initial_storage_fill_factor_psi;
    std::vector<Firm<ModelVariant>*> firms_N;
    Model<ModelVariant>* const model;

  public:
    Sector(Model<ModelVariant>* model_p,
           std::string  name_p,
           const IntType index_p,
           const Ratio& upper_storage_limit_omega_p,
           const Time& initial_storage_fill_factor_psi_p);
    void add_demand_request_D(const Demand& demand_request_D);
    void add_production_X(const Flow& production_X);
    void add_initial_production_X(const Flow& production_X);
    void subtract_initial_production_X(const Flow& production_X);
    void iterate_consumption_and_production();
    void remove_firm(Firm<ModelVariant>* firm);
    inline IntType index() const { return index_; };
    inline operator std::string() const { return name; };
};
}  // namespace acclimate
#endif
