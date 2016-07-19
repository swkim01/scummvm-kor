/* ScummVM - Graphic Adventure Engine
*
* ScummVM is the legal property of its developers, whose names
* are too numerous to list here. Please refer to the COPYRIGHT
* file distributed with this source distribution.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#include "backends/networking/sdl_net/handlers/createdirectoryhandler.h"
#include "backends/fs/fs-factory.h"
#include "backends/networking/sdl_net/handlerutils.h"
#include "backends/networking/sdl_net/localwebserver.h"
#include "common/translation.h"

namespace Networking {

CreateDirectoryHandler::CreateDirectoryHandler() {}

CreateDirectoryHandler::~CreateDirectoryHandler() {}

void CreateDirectoryHandler::handle(Client &client) {
	Common::String path = client.queryParameter("path");
	Common::String name = client.queryParameter("directory_name");

	// check that <path> is not an absolute root
	if (path == "" || path == "/") {
		HandlerUtils::setFilesManagerErrorMessageHandler(client, _("Can't create directory here!"));
		return;
	}

	// transform virtual path to actual file system one
	Common::String prefixToRemove = "", prefixToAdd = "";
	if (!transformPath(path, prefixToRemove, prefixToAdd) || path.empty()) {
		HandlerUtils::setFilesManagerErrorMessageHandler(client, _("Invalid path!"));
		return;
	}

	// check that <path> exists and is directory
	AbstractFSNode *node = g_system->getFilesystemFactory()->makeFileNodePath(path);
	if (!node->exists()) {
		HandlerUtils::setFilesManagerErrorMessageHandler(client, _("Parent directory doesn't exists!"));
		return;
	}
	if (!node->isDirectory()) {
		HandlerUtils::setFilesManagerErrorMessageHandler(client, _("Can't create a directory within a file!"));
		return;
	}

	// check that <directory_name> doesn't exist or is directory
	if (path.lastChar() != '/' && path.lastChar() != '\\') path += '/';
	node = g_system->getFilesystemFactory()->makeFileNodePath(path + name);
	if (node->exists()) {
		if (!node->isDirectory()) {
			HandlerUtils::setFilesManagerErrorMessageHandler(client, _("There is a file with that name in the parent directory!"));
			return;
		}
	} else {
		// create the <directory_name> in <path>
		if (!node->create(true)) {
			HandlerUtils::setFilesManagerErrorMessageHandler(client, _("Failed to create the directory!"));
			return;
		}
	}

	// set redirect on success
	HandlerUtils::setMessageHandler(
		client,
		Common::String::format(
			"%s<br/><a href=\"files?path=%s\">%s</a>",
			_("Directory created successfully!"),
			client.queryParameter("path").c_str(),
			_("Back to parent directory")
		),

		(client.queryParameter("ajax") == "true" ? "/filesAJAX?path=" : "/files?path=") +
			LocalWebserver::urlEncodeQueryParameterValue(client.queryParameter("path"))
	);
}

/// public

ClientHandlerCallback CreateDirectoryHandler::getHandler() {
	return new Common::Callback<CreateDirectoryHandler, Client &>(this, &CreateDirectoryHandler::handle);
}

} // End of namespace Networking
