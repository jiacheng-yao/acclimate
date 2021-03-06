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

#include "output/ConsoleOutput.h"
#include "model/Model.h"
#include "model/Region.h"
#include "model/Sector.h"
#include "scenario/Scenario.h"
#include "variants/ModelVariants.h"
#include "version.h"

namespace acclimate {

template<class ModelVariant>
ConsoleOutput<ModelVariant>::ConsoleOutput(const settings::SettingsNode& settings_p,
                                           Model<ModelVariant>* model_p,
                                           Scenario<ModelVariant>* scenario_p,
                                           const settings::SettingsNode& output_node_p)
    : Output<ModelVariant>(settings_p, model_p, scenario_p, output_node_p) {
    stack = 0;
    out = nullptr;
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::initialize() {
    if (!output_node.has("file")) {
        out = &std::cout;
    } else {
        const std::string filename = output_node["file"].template as<std::string>();
        outfile.reset(new std::ofstream());
        outfile->open(filename.c_str(), std::ofstream::out);
        out = outfile.get();
    }
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_write_header(tm* timestamp, int max_threads) {
    *out << "Start time " << asctime(timestamp) << "Version " << ACCLIMATE_VERSION << "\n"
         << "Max " << max_threads << " threads" << std::endl;
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_write_footer(tm* duration) {
    *out << "\n\nDuration " << mktime(duration) << "s";
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_write_settings() {
    *out << '\n';
    *out << settings;
    *out << '\n';
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_start() {
    *out << "Starting";
    *out << '\n' << '\n' << "Iteration time " << model->time();
    out->flush();
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_iterate_begin() {
    *out << '\n' << '\n' << "Iteration time " << (model->time() + Time(1));
    out->flush();
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_end() {
    *out << '\n' << '\n' << "Ended" << '\n';
    out->flush();
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_write_value(const std::string& name, const FloatType& v) {
    *out << std::setprecision(15) << "\t" << name << "=" << v;
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_start_target(const std::string& name, Sector<ModelVariant>* sector, Region<ModelVariant>* region) {
    stack++;
    *out << '\n' << std::string(2 * stack, ' ') << name << " " << std::string(*sector) << "," << std::string(*region) << ":";
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_start_target(const std::string& name, Sector<ModelVariant>* sector) {
    stack++;
    *out << '\n' << std::string(2 * stack, ' ') << name << " " << std::string(*sector) << ":";
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_start_target(const std::string& name, Region<ModelVariant>* region) {
    stack++;
    *out << '\n' << std::string(2 * stack, ' ') << name << " " << std::string(*region) << ":";
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_start_target(const std::string& name) {
    stack++;
    *out << '\n' << std::string(2 * stack, ' ') << name << ":";
}

template<class ModelVariant>
void ConsoleOutput<ModelVariant>::internal_end_target() {
    stack--;
}

INSTANTIATE_BASIC(ConsoleOutput);
}  // namespace acclimate
