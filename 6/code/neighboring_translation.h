//
// Created by Miguel Chan on 2018/11/28.
//

#ifndef PANORAMA_NEIGHBORING_TRANSLATION_H
#define PANORAMA_NEIGHBORING_TRANSLATION_H

#include <iostream>
#include <vector>

class neighboring_translation {

private:
    const std::vector<std::vector<int>>& adjacents;
public:
    explicit neighboring_translation(const std::vector<std::vector<int>> &adjacents);

    //Write out list of neighboring translations
    std::vector<int> write_out();
};


#endif //PANORAMA_NEIGHBORING_TRANSLATION_H
