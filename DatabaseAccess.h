#pragma once

#include "IDataAccess.h"
#include "sqlite3.h"

class MyDatabaseAccess : public DatabaseAccess
{
public:
	MyDatabaseAccess();
	~MyDatabaseAccess();

	// album related
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;
	void createAlbum(const Album& album) override;
	void deleteAlbum(const std::string& albumName, int userId) override;
	bool doesAlbumExists(const std::string& albumName, int userId) override;
	Album openAlbum(const std::string& albumName) override;
	void closeAlbum(Album& pAlbum) override;
	void printAlbums() override;
	int getAlbumId(std::string albumName);

	// picture related
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) override;
	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	int countTags(int pictureId) override;

	// user related
	void printUsers() override;
	User getUser(int userId) override;
	void createUser(std::string userName) override;
	void deleteUser(const User& user) override;
	bool doesUserExists(int userId) override;
	int getUserId(std::string userName) override;

	// user statistics
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user) override;
	float averageTagsPerAlbumOfUser(const User& user) override;

	// queries
	User getTopTaggedUser() override;
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	bool open() override;
	void close() override;
	void clear() override;

private:
	//album
	friend int albumCallback(void* data, int argc, char** argv, char** azColName);
	
	//user
	friend int userCallback(void* data, int argc, char** argv, char** azColName);

	//picture
	friend int pictureCallback(void* data, int argc, char** argv, char** azColName);
	std::list<Picture> getPicturesOfAlbum(const std::string& albumName);

	//call bake
	friend int printCallback(void* data, int argc, char** argv, char** azColName);
	friend int tagCallback(void* data, int argc, char** argv, char** azColName);
	friend int idCallback(void* data, int argc, char** argv, char** azColName);
	friend int counterCallback(void* data, int argc, char** argv, char** azColName);
	friend int countCallback(void* count, int argc, char** argv, char** azColName);

	static std::list<Album> _tempAlbums;
	static std::list<User> _tempUser;
	static std::list<Picture> _tempPictures;
	static std::list<int> id;
	static std::set<int> _idSet;
	static int counter;

	sqlite3* _db;
};