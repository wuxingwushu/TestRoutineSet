#pragma once
#include <map>

#ifdef MAP_Debug// 开启  事件打印
#include <iostream>
#endif

#ifdef MAP_Timeout// 开启  超时检测
#include <time.h>
#endif


template <typename TData>
struct DataLabel{
    TData* DATA;
    unsigned int Label;
};

template <typename TKey, typename TData>
class ContinuousMap
{
private:
    TKey* KeyS;
    TData* DataS;
#ifdef MAP_Timeout
    clock_t* TimeS;
    clock_t TimeoutTime = 3000;//默认三秒
#endif
    unsigned int Max;//最多容量
    unsigned int Number = 0;//当前容量
    std::map<TKey,DataLabel<TData>> Dictionary;//索引对应数据
public:
    ContinuousMap(unsigned int siez){
        Max = siez;
        KeyS = new TKey[Max];
        DataS = new TData[Max];
#ifdef MAP_Timeout
        TimeS = new clock_t[Max];
#endif
    }

    ~ContinuousMap(){
        delete KeyS;
        delete DataS;
#ifdef MAP_Timeout
        delete TimeS;
#endif
    }

    //添加新 Map
    TData* New(TKey key){
        if (Max == Number)
        {
#ifdef MAP_Debug
            std::cout << "创建 " << key << " 失败：达到上线！" << std::endl;
#endif
            return nullptr;
        }
        DataLabel<TData> dictionary;
        dictionary.DATA = &DataS[Number];
        KeyS[Number] = key;
        dictionary.Label = Number; 
#ifdef MAP_Timeout
        TimeS[Number] = clock();
#endif
        Dictionary.insert(std::make_pair(key, dictionary));
        Number++;
#ifdef MAP_Debug
        std::cout << "创建 " << key << " 成功！" << std::endl;
#endif
        return dictionary.DATA;
    };

    //更加 key 获取对应映射
    TData* Get(TKey key){
        if (Dictionary.find(key) == Dictionary.end())
        {
#ifdef MAP_Debug
            std::cout << "没有 " << key << " ! 创建一个。" << std::endl;
#endif
            return New(key);
        }
#ifdef MAP_Timeout
        DataLabel<TData> dictionary = Dictionary[key];
        TimeS[dictionary.Label] = clock();
        return dictionary.DATA;
#else
        return Dictionary[key].DATA;
#endif
    }

    //销毁 TKey 对应的映射
    void Delete(TKey key){
        if (Dictionary.find(key) == Dictionary.end())
        {
#ifdef MAP_Debug
            std::cout << "释放 " << key << " 失败： 没有找到！" << std::endl;
#endif
            return;
        }
        Number--;
        DataLabel<TData>* keyData = &Dictionary[key];
        if(keyData->Label != KeyS[Number]){
            DataS[keyData->Label] = DataS[Number];
            DataLabel<TData>* EndData = &Dictionary[KeyS[Number]];
            EndData->DATA = &DataS[keyData->Label];
            EndData->Label = keyData->Label;
            KeyS[keyData->Label] = KeyS[Number];
        }
        Dictionary.erase(key);
#ifdef MAP_Debug
        std::cout << "销毁 " << key << " 成功！" << std::endl;
#endif
    }

#ifdef MAP_Timeout
    void TimeoutDetection() {
        clock_t time = clock();
        for (size_t i = 0; i < Number; i++)
        {
            if ((time - TimeS[i]) > TimeoutTime) {
#ifdef MAP_Debug
                std::cout << KeyS[i] << " 超时： 销毁！" << std::endl;
#endif
                Delete(KeyS[i]);
            }
        }
    }

    void SetTimeoutTime(clock_t Time) {
        TimeoutTime = Time;
    }
#endif

    //获取除 Key 以外的所有 Data
    TData* GetKeyData(TKey key) {
        DataLabel<TData>* keyData = &Dictionary[key];
        if (keyData->Label != (Number - 1)) {
            DataLabel<TData>* EndData = &Dictionary[KeyS[Number - 1]];
            std::swap<TKey>(KeyS[keyData->Label], KeyS[Number - 1]);
            std::swap<TData>(DataS[keyData->Label], DataS[Number - 1]);
            std::swap<TData*>(keyData->DATA, EndData->DATA);
            std::swap<unsigned int>(keyData->Label, EndData->Label);
        }
        return DataS;
    }

    //获取除 Key 以外 Data 数量
    unsigned int GetKeyNumber() {
        return (Number - 1);
    }

    //获取除 Key 以外 Data 数量的数据一共多少字节
    unsigned int GetKeyDataSize() {
        return (Number - 1) * sizeof(TData);
    }

    //获取所有 Data
    TData* GetData(){
        return DataS;
    }

    //获取所有 Data 数量
    unsigned int GetNumber(){
        return Number;
    }

    //获取 Data 数量的数据一共多少字节
    unsigned int GetDataSize(){
        return Number * sizeof(TData);
    }
};
