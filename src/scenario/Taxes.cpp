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

#include "scenario/Taxes.h"
#include "variants/ModelVariants.h"

namespace acclimate {

template<class ModelVariant>
Taxes<ModelVariant>::Taxes(const settings::SettingsNode& settings_p, const Model<ModelVariant>* model_p) : Scenario<ModelVariant>(settings_p, model_p) {}

template<>
Time Taxes<VariantBasic>::start() {
    error("Taxes scenario not supported in basic model variant");
}

template<>
Time Taxes<VariantDemand>::start() {
    error("Taxes scenario not supported in demand model variant");
}

template<class ModelVariant>
Time Taxes<ModelVariant>::start() {
    for (auto& region : model->regions_R) {
        region->set_government(new Government<ModelVariant>(region.get()));
    }
    if (settings["scenario"].has("start")) {
        start_time = settings["scenario"]["start"].template as<Time>();
    }
    stop_time = settings["scenario"]["stop"].template as<Time>();
    return start_time;
}

template<>
bool Taxes<VariantBasic>::iterate() {
    return false;
}
template<>
bool Taxes<VariantDemand>::iterate() {
    return false;
}

template<class ModelVariant>
bool Taxes<ModelVariant>::iterate() {
    if (model->time() > stop_time) {
        return false;
    }

    for (const settings::SettingsNode& tax : settings["scenario"]["taxes"].as_sequence()) {
        const Time start_tax = tax["start_tax"].as<Time>();
        const Time full_tax = tax["full_tax"].as<Time>();
        if (model->time() >= start_tax - model->delta_t() && model->time() <= full_tax - model->delta_t()) {
            const Ratio tax_ratio =
                tax["tax_ratio"].as<Ratio>() * (model->time() + 2 * model->delta_t() - start_tax) / (full_tax + model->delta_t() - start_tax);
            const std::string& sector = tax["sector"].as<std::string>();
            if (tax.has("region") && tax["region"].as<std::string>() != "ALL") {
                Region<ModelVariant>* region = model->find_region(tax["region"].as<std::string>());
                if (!region) {
                    error("Could not find region '" << tax["region"] << "'");
                }
                region->government()->define_tax(sector, tax_ratio);
            } else {
                for (auto& region : model->regions_R) {
                    region->government()->define_tax(sector, tax_ratio);
                }
            }
        }
    }

    return true;
}

INSTANTIATE_BASIC(Taxes);
}  // namespace acclimate
