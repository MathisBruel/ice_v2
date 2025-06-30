#pragma once

#include <string>
#include <memory>
#include <map>

class COD_ContentRepo;
class COD_Releases;

class COD_Content
{
public:
    COD_Content(int contentId, std::string contentTitle);
    COD_Content(std::string contentTitle); // Constructeur pour compatibilité
    ~COD_Content();

    void SetContentId(int contentId);
    void SetDatas(std::string contentTitle);

    // Versions const pour toXmlString()
    int GetContentId() const {return this->_contentId;}
    std::string GetContentTitle() const {return this->_contentTitle;}

    // Versions non-const pour MySQLContentRepo (compatibilité)
    int* GetContentIdPtr() {return &this->_contentId;}
    std::string* GetContentTitlePtr() {return &this->_contentTitle;}

    // Méthode pour compatibilité avec l'ancien code (retourne un pointeur)
    int* GetContentId() {return &this->_contentId;}

    // Méthodes pour compatibilité avec l'ancien code
    void AddRelease(COD_Releases* release);
    void DeleteRelease(std::string releaseId);
    COD_Releases* GetRelease(std::string releaseId);
    void CreateRelease(int id_content, int typeMovie, int localisationMovie);

private:
    std::shared_ptr<COD_ContentRepo> _contentRepo;
    int _contentId;
    std::string _contentTitle;
    std::map<std::string, COD_Releases*> _release;
};