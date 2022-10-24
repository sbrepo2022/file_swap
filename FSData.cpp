#include "FSData.h"

QByteArray FSUdpData::getData() {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint64(0); // save some bytes

    out << nickname;
    out << private_key;
    out << is_locked;
    out << num_files;

    out.device()->seek(qint64(0));
    out << qint64(data.size()); // use saved bytes

    return data;
}

bool FSUdpData::setData(QByteArray data) {
    QDataStream in(&data, QIODevice::ReadOnly);

    // check data length
    qint64 size = -1;
    if(in.device()->size() > sizeof(qint64)) {
        in >> size;
    }
    else {
        return false;
    }
    if (in.device()->size() != size)
        return false;

    in >> this->nickname;
    in >> this->private_key;
    in >> this->is_locked;
    in >> this->num_files;

    return true;
}

QByteArray FSDescriptionData::getData() {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint64(0); // save some bytes

    out << action_key;

    out << nickname;
    out << description;

    int filedata_count = filedata.count();

    out << filedata_count;
    for (int i = 0; i < filedata_count; i++) {
        out << filedata[i].full_filename << filedata[i].short_filename << filedata[i].file_size;
    }

    out.device()->seek(qint64(0));
    out << qint64(data.size()); // use saved bytes

    return data;
}

bool FSDescriptionData::setData(QByteArray data) {
    QDataStream in(&data, QIODevice::ReadOnly);

    // check data length
    qint64 size = -1;
    if(in.device()->size() > sizeof(qint64)) {
        in >> size;
    }
    else {
        return false;
    }
    if (in.device()->size() != size)
        return false;

    in >> this->action_key;
    in >> this->nickname;
    in >> this->description;

    int filedata_count;

    in >> filedata_count;
    FileData new_filedata;
    filedata.clear();

    for (int i = 0; i < filedata_count; i++) {
        in >> new_filedata.full_filename >> new_filedata.short_filename >> new_filedata.file_size;
        filedata.append(new_filedata);
    }

    return true;
}
