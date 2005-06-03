/*
 * qca_keystore.h - Qt Cryptographic Architecture
 * Copyright (C) 2003-2005  Justin Karneges <justin@affinix.com>
 * Copyright (C) 2004,2005  Brad Hards <bradh@frogmouth.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef QCA_KEYSTORE_H
#define QCA_KEYSTORE_H

#include "qca_core.h"
#include "qca_cert.h"

namespace QCA
{
	/**
	   Single entry in a KeyStore

	   This is a container for any kind of object in a KeyStore
	*/
	class QCA_EXPORT KeyStoreEntry : public Algorithm
	{
	public:
		/**
		   The type of entry in the KeyStore
		*/
		enum Type
		{
			TypeKeyBundle,
			TypeCertificate,
			TypeCRL,
			TypePGPSecretKey,
			TypePGPPublicKey
		};

		/**
		   Create an empty KeyStoreEntry
		*/
		KeyStoreEntry();

		/**
		   Standard copy constructor

		   \param from the source entry
		*/
		KeyStoreEntry(const KeyStoreEntry &from);
		~KeyStoreEntry();

		/**
		   Standard assignment operator

		   \param from the source entry
		*/
		KeyStoreEntry & operator=(const KeyStoreEntry &from);

		/**
		   Test if this key is empty (null)
		*/
		bool isNull() const;

		/**
		   Determine the type of key stored in this object 
		*/
		Type type() const;

		/**
		   The name associated with the key stored in this object
		*/
		QString name() const;

		/**
		   The ID associated with the key stored in this object
		*/
		QString id() const;

		/**
		   If a KeyBundle is stored in this object, return that
		   bundle.
		KeyBundle keyBundle() const;

		/**
		   If a Certificate is stored in this object, return that
		   certificate.
		*/
		Certificate certificate() const;

		/**
		   If a CRL is stored in this object, return the value
		   of the CRL
		*/
		CRL crl() const;

		/**
		   If the key stored in this object is a private
		   PGP key, return the contents of that key.
		*/
		PGPKey pgpSecretKey() const;

		/**
		   If the key stored in this object is either an 
		   public or private PGP key, extract the public key
		   part of that PGP key.
		*/
		PGPKey pgpPublicKey() const;
	};

	/**
	   General purpose key storage object

	   Examples of use of this are:
	    -  systemstore:          System TrustedCertificates
	    -  accepted self-signed: Application TrustedCertificates
	    -  apple keychain:       User Identities
	    -  smartcard:            SmartCard Identities
	    -  gnupg:                PGPKeyring Identities,PGPPublicKeys
	*/
	class QCA_EXPORT KeyStore : public QObject, public Algorithm
	{
		Q_OBJECT
	public:
		/**
		   The type of keystore
		*/
		enum Type
		{
			System,      ///< objects such as root certificates
			User,        ///< objects such as  Apple Keychain, KDE Wallet
			Application, ///< for caching accepted self-signed certificates
			SmartCard,   ///< for smartcards
			PGPKeyring   ///< for a PGP keyring
		};

		/**
		   The keystore Type
		*/
		Type type() const;

		/**
		   The name associated with the keystore
		*/
		QString name() const;

		/**
		   The ID associated with the keystore
		*/
		QString id() const;

		/**
		   Test if the KeyStore is writeable or not

		   \return true if the KeyStore is read-only
		*/
		bool isReadOnly() const;

		/**
		   A list of the KeyStoreEntry objects in this store
		*/
		QList<KeyStoreEntry> entryList() const;

		/**
		   test if the KeyStore holds trusted certificates (and CRLs)
		*/
		bool holdsTrustedCertificates() const; // Certificate and CRL

		/**
		   test if the KeyStore holds identities (eg KeyBundle or PGPSecretKey)
		*/
		bool holdsIdentities() const;

		/**
		   test if the KeyStore holds PGPPublicKey objects
		*/
		bool holdsPGPPublicKeys() const;

		/**
		   Add a entry to the KeyStore

		   \param kb the KeyBundle to add to the KeyStore
		*/
		bool writeEntry(const KeyBundle &kb);

		/**
		   \overload

		   \param cert the Certificate to add to the KeyStore
		*/
		bool writeEntry(const Certificate &cert);

		/**
		   \overload

		   \param crl the CRL to add to the KeyStore
		*/
		bool writeEntry(const CRL &crl);

		/**
		   \overload

		   \param key the PGPKey to add to the KeyStore

		   \return a ref to the key in the keyring
		*/
		PGPKey writeEntry(const PGPKey &key);

		/**
		   Delete the a specified KeyStoreEntry from this KeyStore

		   \param id the ID for the entry to be deleted
		*/
		bool removeEntry(const QString &id);

		/**
		   Provide a passphrase for the KeyStore

		   \param passphrase the passphrase to use
		*/
		void submitPassphrase(const QSecureArray &passphrase);

	signals:
		/**
		   Emitted when the KeyStore is changed
		*/
		void updated();

		/**
		   Emitted when the KeyStore becomes unavailable
		*/
		void unavailable();

		/**
		   Emitted when the KeyStore requires a passphrase
		*/
		void needPassphrase();

	private:
		friend class KeyStoreManager;
		friend class KeyStoreManagerPrivate;
		KeyStore();
		~KeyStore();
	};

	// use this to get access to keystores and monitor for their activity
	/**
	   Access keystores, and monitor keystores for changes
	*/
	class QCA_EXPORT KeyStoreManager : public QObject
	{
		Q_OBJECT
	public:

		/**
		   Obtain a specific KeyStore

		   \param id the identification for the key store
		*/
		KeyStore *keyStore(const QString &id) const;

		/**
		   A list of the all the key stores
		*/
		QList<KeyStore*> keyStores() const;

		/**
		   The number of key stores that are currently active
		*/
		int count() const;

		/**
		   The diagnotistic result of key store operations, such as
		   warnings and errors
		*/
		QString diagnosticText() const;

	signals:
		/**
		   emitted when a new key store becomes available
		*/
		void keyStoreAvailable(const QString &id);

	private:
		friend class KeyStoreManagerPrivate;
		KeyStoreManagerPrivate *d;

		friend class Global;
		KeyStoreManager();
		~KeyStoreManager();

		void scan() const;
	};
}

#endif
