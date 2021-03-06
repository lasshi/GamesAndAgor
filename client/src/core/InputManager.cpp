#include "core/InputManager.hpp"

void InputManager::pressKey(unsigned int key) {
    m_keys[key] = true; // set true for this key
    m_lastCharacter = int(key);
}
void InputManager::releaseKey(unsigned int key) {
    m_keys[key] = false; // set false for this key
}
bool InputManager::isKeyDown(unsigned int key) {
    auto it = m_keys.find(key); // find id from keys
    if( it != m_keys.end() ) return it->second; // if key exists in map, return falue
    return false;    // otherwise it has never been set, so it cannot be down
}
bool InputManager::isKeyPressed(unsigned int key) {
    // if key is down, but wasnt down last time we checked
    if(isKeyDown(key) && !wasKeyDown(key)) return true;
    return false;
}
bool InputManager::wasKeyDown(unsigned int key) {
    // was key down last time
    auto it = m_previousKeys.find(key);
    if(it != m_previousKeys.end())
        return it->second;
    else
        return false;
}
void InputManager::update() {
    for (auto& it : m_keys)
        m_previousKeys[it.first] = it.second; // copy whole keymap to previous
}
