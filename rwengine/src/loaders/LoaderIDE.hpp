#ifndef _RWENGINE_LOADERIDE_HPP_
#define _RWENGINE_LOADERIDE_HPP_

#include <map>
#include <memory>
#include <string>

#include <data/PedData.hpp>
#include <data/ModelData.hpp>

class LoaderIDE {
public:
    enum SectionTypes {
        NONE,
        OBJS,
        TOBJ,
        PEDS,
        CARS,
        HIER,
        TWODFX,
        PATH,
    };

    // Load the IDE data into memory
    bool load(const std::string& filename, const PedStatsList& stats);

    bool load(std::istream& stream, const PedStatsList& stats);

    /**
     * @brief objects loaded during the call to load()
     */
    std::map<ModelID, std::unique_ptr<BaseModelInfo>> objects;
};

#endif
