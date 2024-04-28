//
// Created by petrs on 27.04.2024.
//

#ifndef GAMM_GAMM_H
#define GAMM_GAMM_H


class GAMM {

public:
    static void updateInlet(std::unordered_map<int, Cell>& cells);
    static void updateOutlet(std::unordered_map<int, Cell>& cells);
    static void updateWalls(std::unordered_map<int, Cell>& cells, const std::unordered_map<std::pair<int, int>, Interface, pair_hash>& faces);
    static void updateBounds(std::unordered_map<int, Cell>& cells, const std::unordered_map<std::pair<int, int>, Interface, pair_hash>& faces);

};


#endif //GAMM_GAMM_H
