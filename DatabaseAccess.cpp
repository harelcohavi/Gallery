#pragma once

#include "sqlite3.h"
#include <io.h>
#include <string>
#include <stdexcept>

#include "DatabaseAccess.h"

std::list<Album> MyDatabaseAccess::_tempAlbums;
std::list<User> MyDatabaseAccess::_tempUser;
std::list<Picture> MyDatabaseAccess::_tempPictures;
std::list<int> MyDatabaseAccess::id;
std::set<int> MyDatabaseAccess::_idSet;
int MyDatabaseAccess::counter = 0;

MyDatabaseAccess::MyDatabaseAccess()
{

}
MyDatabaseAccess::~MyDatabaseAccess()
{
}

// album related
const std::list<Album> MyDatabaseAccess::getAlbums()
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	sqlStatement = "SELECT * FROM ALBUMS;";

	this->_tempAlbums.clear();

	res = sqlite3_exec(_db, sqlStatement.c_str(), albumCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		throw std::invalid_argument(errMessage);
	}

	return this->_tempAlbums;
}
const std::list<Album> MyDatabaseAccess::getAlbumsOfUser(const User& user)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	if(doesUserExists(user.getId()))
	{
		sqlStatement = "SELECT * FROM ALBUMS WHERE USER_ID=" + std::to_string(user.getId()) + ";";

		this->_tempAlbums.clear();

		res = sqlite3_exec(_db, sqlStatement.c_str(), albumCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "filed" << std::endl;
			if (errMessage)
			{
				throw std::invalid_argument(errMessage);
			}

			this->_tempAlbums.clear();
			return this->_tempAlbums;
		}

		return this->_tempAlbums;
	}
	else
	{
		
	}
	
}
void MyDatabaseAccess::createAlbum(const Album& album)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	
	sqlStatement = "INSERT INTO ALBUMS (NAME, USER_ID, CREATION_DATE) VALUES ('" + album.getName() + "'," + std::to_string(album.getOwnerId()) + ",'" + album.getCreationDate() + "');";

	res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Create album filed" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}
}
void MyDatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;
	int albumId = getAlbumId(albumName);
	std::list<Picture> pic = getPicturesOfAlbum(albumName);


	//delete album tags pictures of album
	for (auto it = pic.begin(); it != pic.end(); it++)
	{
		sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID = \"" + std::to_string(it->getId()) + "\";";

		res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "filed" << std::endl;
			if (errMessage)
			{
				throw std::invalid_argument(errMessage);
			}
			return;
		}
	}

	//delete album pictures
	sqlStatement = "DELETE FROM PICTURES WHERE ALBUM_ID = \"" + std::to_string(albumId) + "\";";

	res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "filed" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
		return;
	}

	//delete album
	sqlStatement = "DELETE FROM ALBUMS WHERE NAME = \"" + albumName + "\";";

	res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "filed" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
		return;
	}


}
bool MyDatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	this->_tempAlbums.clear();

	sqlStatement = "SELECT * FROM AlBUMS WHERE NAME=\"" + albumName + "\";";

	res = sqlite3_exec(_db, sqlStatement.c_str(), albumCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "filed" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
		return false;
	}

	return this->_tempAlbums.size() == 1;
}
Album MyDatabaseAccess::openAlbum(const std::string& albumName)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;
	Album a;

	this->_tempAlbums.clear();

	sqlStatement = "SELECT * FROM AlBUMS WHERE NAME=\"" + albumName + "\";";

	res = sqlite3_exec(_db, sqlStatement.c_str(), albumCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Open album filed" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}

		
		return a;
	}

	a = this->_tempAlbums.front();

	std::list<Picture> pictures = getPicturesOfAlbum(albumName);

	for (auto it = pictures.begin(); it != pictures.end(); it++)
	{
		a.addPicture(*it);
	}

	return a;
}
void MyDatabaseAccess::closeAlbum(Album & pAlbum)
{
	//I think that this function should do nothing
}
void MyDatabaseAccess::printAlbums()
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	sqlStatement = "SELECT * FROM ALBUMS;";

	res = sqlite3_exec(_db, sqlStatement.c_str(), printCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "filed" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}
}

int MyDatabaseAccess::getAlbumId(std::string albumName)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	Album album;

	this->_tempAlbums.clear();
	id.clear();

	sqlStatement = "SELECT * FROM ALBUMS WHERE NAME=\"" + albumName + "\";";

	res = sqlite3_exec(_db, sqlStatement.c_str(), idCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Get album id filed" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
		return 0;
	}
	
	return id.front();
}

int albumCallback(void* data, int argc, char** argv, char** azColName)
{
	Album temp;

	int i = 0;

	for (i = 0; i < argc; i++)
	{
		if (!strcmp(azColName[i],"USER_ID"))
		{
			temp.setOwner(std::stoi(argv[i]));
		}
		else if (!strcmp(azColName[i],"NAME"))
		{
			temp.setName(argv[i]);
		}
		else if (!strcmp(azColName[i],"CREATION_DATE"))
		{
			temp.setCreationDate(argv[i]);
		}
	}

	MyDatabaseAccess::_tempAlbums.push_back(temp);
	

	return 0;
}

// picture related
void MyDatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	
	sqlStatement = "INSERT INTO PICTURES (NAME, LOCATION, CREATION_DATE, ALBUM_ID) VALUES ('" + picture.getName() + "','" + picture.getPath() + "','" + picture.getCreationDate() + "'," + std::to_string(getAlbumId(albumName)) + ");";

	res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Add picture failed" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}
}
void MyDatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	id.clear();
	//get picture id
	sqlStatement = "SELECT * FROM PICTURES WHERE NAME=\"" + pictureName + "\";";
	res = sqlite3_exec(_db, sqlStatement.c_str(), idCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Get picture id filed" << std::endl;
		if (errMessage)
		{
			throw errMessage;
		}
		return;
	}

	//deelte all picture tags
	sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID = \"" + std::to_string(id.front()) + "\";";

	res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "filed" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
		return;
	}

	//delete picture
	sqlStatement = "DELETE FROM PICTURES WHERE ID = " + std::to_string(id.front()) + ";";
	errMessage = nullptr;

	res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Delete picture failed" << std::endl;
		if (errMessage)
		{
			std::cout << errMessage << std::endl;
		}
	}

}
void MyDatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	id.clear();

	sqlStatement = "SELECT * FROM PICTURES WHERE NAME=\"" + pictureName + "\";";
	res = sqlite3_exec(_db, sqlStatement.c_str(), idCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Get picture id filed" << std::endl;
		if (errMessage)
		{
			throw errMessage;
		}
		return;
	}


	sqlStatement = "INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (" + std::to_string(id.front()) + "," + std::to_string(userId) + ");";
	errMessage = nullptr;

	res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Add tag failed" << std::endl;
		if (errMessage)
		{
			std::cout << errMessage << std::endl;
			errMessage = nullptr;
			throw errMessage;
		}
	}
}
void MyDatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	id.clear();

	sqlStatement = "SELECT * FROM PICTURES WHERE NAME=\"" + pictureName + "\";";
	res = sqlite3_exec(_db, sqlStatement.c_str(), idCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
		return;
	}


	sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID = " + std::to_string(id.front()) + " AND USER_ID =" + std::to_string(userId) + ";";
	errMessage = nullptr;

	res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}
}
std::list<Picture> MyDatabaseAccess::getPicturesOfAlbum(const std::string& albumName)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	this->_tempPictures.clear();

	sqlStatement = "SELECT * FROM PICTURES WHERE ALBUM_ID=" + std::to_string(getAlbumId(albumName)) + ";";

	res = sqlite3_exec(this->_db, sqlStatement.c_str(), pictureCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Failed counting tags" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}

	for (auto it = this->_tempPictures.begin(); it != this->_tempPictures.end(); it++)
	{
		sqlStatement = "SELECT * FROM TAGS WHERE PICTURE_ID=" + std::to_string(it->getId()) + ";";
		
		this->_idSet.clear();
		
		res = sqlite3_exec(this->_db, sqlStatement.c_str(), tagCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "Failed counting tags" << std::endl;
			if (errMessage)
			{
				throw std::invalid_argument(errMessage);
			}
		}

		for (auto it2 = this->_idSet.begin(); it2 != this->_idSet.end(); it2++)
		{
			it->tagUser(*it2);
		}
	}

	return this->_tempPictures;
}
int MyDatabaseAccess::countTags(int pictureId)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	id.clear();

	sqlStatement = "SELECT COUNT(*) FROM TAGS WHERE PICTURE_ID =" + std::to_string(pictureId) + ";";
	res = sqlite3_exec(_db, sqlStatement.c_str(), idCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw errMessage;
		}
	}

	return this->id.front();
}

int pictureCallback(void* data, int argc, char** argv, char** azColName)
{
	Picture pic(1, "");


	if (argc >= 4)
	{
		pic.setId(std::atoi(argv[0]));
		pic.setName(argv[1]);
		pic.setPath(argv[2]);
		pic.setCreationDate(argv[3]);

		MyDatabaseAccess::_tempPictures.push_back(pic);
	}

	return 0;
}
int tagCallback(void* data, int argc, char** argv, char** azColName)
{
	int i = 0;

	if (argc >= 2)
	{
		for (i = 0; i < argc; i++)
		{
			if (!strcmp(azColName[i], "USER_ID"))
			{
				MyDatabaseAccess::_idSet.insert(std::stoi(argv[i]));
			}
		}
	}
	return 0;
}


// user related
void MyDatabaseAccess::printUsers()
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	sqlStatement = "SELECT * FROM USERS;";

	res = sqlite3_exec(_db, sqlStatement.c_str(), printCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}
}
User MyDatabaseAccess::getUser(int userId)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	this->_tempUser.clear();

	sqlStatement = "SELECT * FROM USERS WHERE ID=" + std::to_string(userId) + ";";

	res = sqlite3_exec(_db, sqlStatement.c_str(), userCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}

	return this->_tempUser.front();
}
void MyDatabaseAccess::createUser(std::string userName)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	sqlStatement = "INSERT INTO USERS (NAME) VALUES ('" + userName + "');";

	res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}
}
void MyDatabaseAccess::deleteUser(const User& user)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;
	std::list<Album> albums = getAlbumsOfUser(user);

	//delete user albums, pictures and tags of his pictures
	for (auto it = albums.begin(); it != albums.end(); it++)
	{
		deleteAlbum(it->getName(), user.getId());
	}

	//delete user tags
	sqlStatement = "DELETE FROM TAGS WHERE USER_ID = " + std::to_string(user.getId()) + ";";

	res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}

	//delete user
	sqlStatement = "DELETE FROM USERS WHERE ID = " + std::to_string(user.getId()) + ';';

	res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}
}
bool MyDatabaseAccess::doesUserExists(int userId)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	this->_tempUser.clear();

	sqlStatement = "SELECT * FROM USERS WHERE ID=" + std::to_string(userId) + ";";

	res = sqlite3_exec(_db, sqlStatement.c_str(), userCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}

	return this->_tempUser.size() >= 1;
}
int MyDatabaseAccess::getUserId(std::string userName)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	this->_tempUser.clear();

	sqlStatement = "SELECT * FROM USERS WHERE NAME='" + userName + "';";

	res = sqlite3_exec(_db, sqlStatement.c_str(), userCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}

	return this->_tempUser.back().getId();
}

int userCallback(void* data, int argc, char** argv, char** azColName)
{
	User temp(1, "");

	int i = 0;

	for (i = 0; i < argc; i++)
	{
		if (!strcmp(azColName[i],"ID"))
		{
			temp.setId(std::stoi(argv[i]));
		}
		else if (!strcmp(azColName[i],"NAME"))
		{
			temp.setName(argv[i]);
		}

	}

	MyDatabaseAccess::_tempUser.push_back(temp);


	return 0;
}


// user statistics
int MyDatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	getAlbumsOfUser(user);

	return this->_tempAlbums.size();
}
int MyDatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	std::list<Album> albums = getAlbums();
	bool fountInAlbum = false;

	counter = 0;

	for (auto it = albums.begin(); it != albums.end(); it++)
	{
		fountInAlbum = false;
		std::list<Picture> pictures = getPicturesOfAlbum(it->getName());

		for (auto it2 = pictures.begin(); it2 != pictures.end() && !fountInAlbum; it2++)
		{
			std::set<int> tags = it2->getUserTags();
			for (auto it3 = tags.begin(); it3 != tags.end() && !fountInAlbum; it3++)
			{
				if (*it3 == user.getId())
				{
					fountInAlbum = true;
					counter++;
				}
			}
		}
	}

	return counter;
}
int MyDatabaseAccess::countTagsOfUser(const User& user)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	counter = 0;

	sqlStatement = "SELECT * FROM TAGS WHERE USER_ID=" + std::to_string(user.getId()) + ";";

	res = sqlite3_exec(_db, sqlStatement.c_str(), counterCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Failed counting tags" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}

	return counter;
}
float MyDatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	int albumsTaggedCount = countAlbumsTaggedOfUser(user);

	if (0 == albumsTaggedCount) {
		return 0;
	}

	return static_cast<float>(countTagsOfUser(user)) / albumsTaggedCount;
}

int counterCallback(void* data, int argc, char** argv, char** azColName)
{
	MyDatabaseAccess::counter++;
	return 0;
}

// queries
User MyDatabaseAccess::getTopTaggedUser()
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;
	int count = 0;

	id.clear();

	sqlStatement = "SELECT COUNT(*) FROM TAGS";
	sqlite3_exec(_db, sqlStatement.c_str(), countCallback, &count, &errMessage);

	if (count >= 1)
	{

		sqlStatement = "SELECT USER_ID, COUNT(*) as num_tags FROM TAGS GROUP BY USER_ID ORDER BY num_tags DESC LIMIT 1;";

		res = sqlite3_exec(_db, sqlStatement.c_str(), idCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			if (errMessage)
			{
				throw std::invalid_argument(errMessage);
			}
		}

		return getUser(id.front());
	}
	else
	{
		throw std::invalid_argument("There is no tags");
	}
}
Picture MyDatabaseAccess::getTopTaggedPicture()
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;
	int count = 0;

	id.clear();

	sqlStatement = "SELECT COUNT(*) FROM TAGS";
	sqlite3_exec(_db, sqlStatement.c_str(), countCallback, &count, &errMessage);

	if (count >= 1)
	{

		sqlStatement = "SELECT PICTURE_ID, COUNT(*) as num_tags FROM TAGS GROUP BY USER_ID ORDER BY num_tags DESC LIMIT 1;";

		res = sqlite3_exec(_db, sqlStatement.c_str(), idCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			if (errMessage)
			{
				throw std::invalid_argument(errMessage);
			}
		}

		sqlStatement = "SELECT * FROM PICTURES WHERE ID = " + std::to_string(id.front()) + ";";
		errMessage = nullptr;

		res = sqlite3_exec(_db, sqlStatement.c_str(), pictureCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			throw std::invalid_argument(errMessage);
		}

		return _tempPictures.front();
	}
	else
	{
		throw std::invalid_argument("There is no tags");
	}
}
std::list<Picture> MyDatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	this->_tempPictures.clear();
	id.clear();

	sqlStatement = "SELECT PICTURE_ID FROM TAGS WHERE USER_ID =" + std::to_string(user.getId()) + ";";

	res = sqlite3_exec(_db, sqlStatement.c_str(), idCallback, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}

	for (auto it = id.begin(); it != id.end();it++)
	{
		sqlStatement = "SELECT * FROM PICTURES WHERE ID =" + std::to_string(*it) + ";";

		res = sqlite3_exec(_db, sqlStatement.c_str(), pictureCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			if (errMessage)
			{
				throw std::invalid_argument(errMessage);
			}
		}
	}

	for (auto it = this->_tempPictures.begin(); it != this->_tempPictures.end(); it++)
	{
		sqlStatement = "SELECT * FROM TAGS WHERE PICTURE_ID=" + std::to_string(it->getId()) + ";";

		this->_idSet.clear();

		res = sqlite3_exec(this->_db, sqlStatement.c_str(), tagCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "Failed counting tags" << std::endl;
			if (errMessage)
			{
				throw std::invalid_argument(errMessage);
			}
		}

		for (auto it2 = this->_idSet.begin(); it2 != this->_idSet.end(); it2++)
		{
			it->tagUser(*it2);
		}
	}

	return this->_tempPictures;
}

bool MyDatabaseAccess::open()
{
	std::string dbFileName = "Gallery.sqlite";
	char* errMessage = nullptr;
	char* sqlStatement = "";
	int res = 0;

	int doesFileExist = _access(dbFileName.c_str(), 0);

	res = sqlite3_open(dbFileName.c_str(), &this->_db);
	if (res != SQLITE_OK)
	{
		this->_db = nullptr;
		std::cout << "Failed to open DB" << std::endl;
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
		return 1;
	}

	if (doesFileExist != 0)
	{
		Album album;

		this->_tempAlbums.clear();

		sqlStatement = "CREATE TABLE USERS (ID INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , NAME TEXT NOT NULL);";
		errMessage = nullptr;

		res = sqlite3_exec(_db, sqlStatement, albumCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "Create users table filed" << std::endl;
			throw std::invalid_argument(errMessage);
		}

		sqlStatement = "CREATE TABLE TAGS(PICTURE_ID INTEGER NOT NULL, USER_ID INTEGER NOT NULL, PRIMARY KEY(PICTURE_ID,USER_ID),  FOREIGN KEY(PICTURE_ID ) REFERENCES PICTURES (ID), FOREIGN KEY(USER_ID ) REFERENCES USERS (ID));";
		errMessage = nullptr;

		res = sqlite3_exec(_db, sqlStatement, albumCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "Create tags table filed" << std::endl;
			throw std::invalid_argument(errMessage);
		}

		sqlStatement = "CREATE TABLE ALBUMS (ID INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , NAME TEXT NOT NULL , USER_ID INTEGER NOT NULL , CREATION_DATE TEXT NOT NULL,  FOREIGN KEY(USER_ID) REFERENCES USERS (ID));";
		errMessage = nullptr;

		res = sqlite3_exec(_db, sqlStatement, albumCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "Create albums table filed" << std::endl;
			throw std::invalid_argument(errMessage);
		}

		sqlStatement = "CREATE TABLE PICTURES ( ID  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,  NAME TEXT  NOT NULL, LOCATION TEXT NOT NULL,CREATION_DATE TEXT NOT NULL, ALBUM_ID INTEGER NOT NULL,  FOREIGN KEY(ALBUM_ID ) REFERENCES ALBUMS (ID));";
		errMessage = nullptr;

		res = sqlite3_exec(_db, sqlStatement, albumCallback, nullptr, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "Create pictures table filed" << std::endl;
			throw std::invalid_argument(errMessage);
		}

	}
}
void MyDatabaseAccess::close()
{
	sqlite3_close(this->_db);
	this->_db = nullptr;
}
void MyDatabaseAccess::clear()
{
	std::string sqlStatement = "";
	char* errMessage = nullptr;
	int res = 0;
	int i = 0;

	sqlStatement = "DELETE FROM ALBUMS";

	res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}

	sqlStatement = "DELETE FROM USERS";

	res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}

	sqlStatement = "DELETE FROM PICTURES";

	res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}

	sqlStatement = "DELETE FROM TAGS";

	res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		if (errMessage)
		{
			throw std::invalid_argument(errMessage);
		}
	}
}

int idCallback(void* data, int argc, char** argv, char** azColName)
{
	int i = 0;

	for (i = 0; i < argc; i++)
	{
		if (!strcmp(azColName[i],"ID") || !strcmp(azColName[i], "USER_ID") || !strcmp(azColName[i], "PICTURE_ID") || !strcmp(azColName[i], "ALBUM_ID") || !strcmp(azColName[i], "COUNT(*)"))
		{
			MyDatabaseAccess::id.push_back(std::stoi(argv[i]));
		}
	}

	return 0;
}
int printCallback(void* data, int argc, char** argv, char** azColName)
{
	int i = 0;

	for (i = 0; i < argc; i++)
	{
		std::cout << azColName[i] << ": " << argv[i];
		if (i + 1 == argc)
		{
			std::cout << std::endl;
		}
		else
		{
			std::cout << ", ";
		}
	}

	return 0;
}
int countCallback(void* count, int argc, char** argv, char** azColName)
{
	int* c = static_cast<int*>(count);
	*c = atoi(argv[0]);
	return 0;
}