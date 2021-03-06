/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    Util/Sharing.h
 *
 * Helper functions to aid sharing functionality in Tizen apps.
 *
 * Created on:  Mar 28, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#ifndef TFC_SHARING_H_
#define TFC_SHARING_H_

#include <string>
#include <efl_extension.h>

/**
 * Wrapper for Tizen App Control functions.
 */
namespace TFC { namespace Util {
	/**
	 * Method to share a text by bluetooth, memo, etc.
	 *
	 * @param str String reference that will be shared.
	 */
	void ShareString(const std::string& str);

	/**
	 * Method to store text to clipboard.
	 *
	 * @param str String reference that will be copied to clipboard.
	 * @param source Evas object that holds the text.
	 *
	 * @return True if the operation succeeded, false if failed.
	 */
	bool CopyStringToClipboard(const std::string& str, Evas_Object* source);

	/**
	 * Method to open native web browser and point it to a particular URL.
	 *
	 * @param url Address that will be opened in the browser.
	 */
	void OpenURL(const std::string& url);

	/**
	 * Method to launch native multimedia viewer applications, e.g. video player.
	 *
	 * @param uri Address or path of the multimedia file.
	 * @param mimeType MIME type of the file, will affect which viewer application will be called.
	 */
	void LaunchViewer(const std::string& uri, const std::string& mimeType);

	/**
	 * Method to compose an email with one recipient.
	 *
	 * @param recipient Email address of the recipient.
	 * @param subject Subject of the email.
	 * @param text Body of the email.
	 */
	void ComposeMailForOne(const std::string& recipient, const std::string& subject, const std::string& text);

}
}


#endif /* SHARING_H_ */
