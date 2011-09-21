CREATE TABLE Area (
    idArea int(11) unsigned NOT NULL auto_increment,
    name varchar(255) default NULL,
    UNIQUE KEY (name),
    PRIMARY KEY(idArea)
) TYPE=InnoDB;

CREATE TABLE Category (
    idCategory int(11) unsigned NOT NULL auto_increment,
    name varchar(255) default NULL,
    UNIQUE KEY (name),
    PRIMARY KEY(idCategory)
) TYPE=InnoDB;

CREATE TABLE Customer (
    idCustomer int(11) unsigned NOT NULL auto_increment,
    lastName varchar(255) default NULL,
    name varchar(255) default NULL,
    address varchar(255) default NULL,
    location varchar(255) default NULL,
    cellphone varchar(63) default NULL,
    phone varchar(63) default NULL,
    email varchar(255) default NULL,
    PRIMARY KEY (idCustomer)
) TYPE=InnoDB;

CREATE TABLE Employed (
    idEmployed int(11) unsigned NOT NULL auto_increment,
    lastName varchar(255) default NULL,
    name varchar(255) default NULL,
    idCategory int(11) unsigned NOT NULL,
    dni varchar(31) default NULL,
    address varchar(255) default NULL,
    location varchar(255) default NULL,
    cellphone varchar(63) default NULL,
    phone varchar(63) default NULL,
    rating int(2) default NULL,
    UNIQUE KEY (dni),
    PRIMARY KEY (idEmployed),
    FOREIGN KEY (idCategory) REFERENCES Category (idCategory)
) TYPE=InnoDB;

CREATE TABLE Supplier (
    idSupplier int(11) unsigned NOT NULL auto_increment,
    name varchar(255) default NULL,
    idArea int(11) unsigned NOT NULL,
    address varchar(255) default NULL,
    location varchar(255) default NULL,
    phone varchar(63) default NULL,
    fax varchar(63) default NULL,
    email varchar(255) default NULL,
    contactName varchar(255) default NULL,
    contactPhone varchar(32) default NULL,
    PRIMARY KEY (idSupplier),
    FOREIGN KEY (idArea) REFERENCES Area (idArea)
) TYPE=InnoDB;

CREATE TABLE Building (
    idBuilding int(11) unsigned NOT NULL auto_increment,
    name varchar(255) default NULL,
    idCustomer int(11) unsigned NOT NULL,
    address varchar(255) default NULL,
    location varchar(255) default NULL,
    type varchar(63) default NULL,
    UNIQUE(name),
    PRIMARY KEY (idBuilding),
    FOREIGN KEY (idCustomer) REFERENCES Customer (idCustomer)
) TYPE=InnoDB;

CREATE TABLE Receipt (
    idReceipt int(11) unsigned NOT NULL auto_increment,
    idEmployed int(11) unsigned NOT NULL,
    idBuilding int(11) unsigned NOT NULL,
    date date NOT NULL,
    type varchar(255) default NULL,
    concept varchar(255) default NULL,
    amount float(13,2) default NULL,
    PRIMARY KEY (idReceipt),
    FOREIGN KEY (idEmployed) REFERENCES Employed (idEmployed),
    FOREIGN KEY (idBuilding) REFERENCES Building (idBuilding)
) TYPE=InnoDB;

-- ALTER TABLE Receipt ADD ownername varchar(255) default NULL AFTER idBuilding;
