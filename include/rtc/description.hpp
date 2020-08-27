/**
 * Copyright (c) 2019 Paul-Louis Ageneau
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef RTC_DESCRIPTION_H
#define RTC_DESCRIPTION_H

#include "candidate.hpp"
#include "include.hpp"

#include <iostream>
#include <map>
#include <optional>
#include <vector>

namespace rtc {


class Description {
public:

    enum class Type { Unspec = 0, Offer = 1, Answer = 2 };
	enum class Role { ActPass = 0, Passive = 1, Active = 2 };
    enum Direction {
        SendOnly,
        RecvOnly,
        SendRecv,
        Unknown
    };

    Description(const string &sdp, const string &typeString = "");
	Description(const string &sdp, Type type);
	Description(const string &sdp, Type type, Role role);
    Description(): Description(""){};

	Type type() const;
	string typeString() const;
	Role role() const;
	string roleString() const;
	string dataMid() const;
	string bundleMid() const;
	std::optional<string> fingerprint() const;
	std::optional<uint16_t> sctpPort() const;
	std::optional<size_t> maxMessageSize() const;
	bool ended() const;

	void hintType(Type type);
	void setDataMid(string mid);
	void setFingerprint(string fingerprint);
	void setSctpPort(uint16_t port);
	void setMaxMessageSize(size_t size);

	void addCandidate(Candidate candidate);
	void endCandidates();
	std::vector<Candidate> extractCandidates();

	bool hasMedia() const;
	void addMedia(const Description &source);

	operator string() const;
	string generateSdp(const string &eol) const;
	string generateDataSdp(const string &eol) const;

	// Data
    struct Data {
        string mid;
        std::optional<uint16_t> sctpPort;
        std::optional<size_t> maxMessageSize;
    };

    // Media (non-data)
    struct Media {
        Media(const string &lines);
        string type;
        string description;
        string mid;
        std::vector<string> attributes;
        std::vector<string> attributesl;
        int bAS = -1;

        struct RTPMap {
            RTPMap(const string &mLine);

            int pt;
            string format;
            int clockRate;
            string encParams;

            std::vector<string> rtcpFbs;
            std::vector<string> fmtps;

            void removeFB(const string& string);
            void addFB(const string& string);
        };

        std::unordered_map<int, RTPMap> rtpMap;

        Media::RTPMap& getFormat(int fmt);
        Media::RTPMap& getFormat(const string& fmt);
        void removeFormat(const string &fmt);

        Direction getDirection();
        void setDirection(Direction dir);

        void addVideoCodec(int payloadType, const string& codec);
        void addH264Codec(int payloadType);
        void addVP8Codec(int payloadType);
        void addVP9Codec(int payloadType);

        void setBitrate(int bitrate);
        int getBitrate() const;
    };

    std::_Rb_tree_iterator<std::pair<const int, Media>> getMedia(int mLine);

    Media & addAudioMedia();

    Media &addVideoMedia(Direction direction=Direction::RecvOnly);

private:
	Type mType;
	Role mRole;
	string mSessionId;
	string mIceUfrag, mIcePwd;
	std::optional<string> mFingerprint;

	Data mData;
  
	std::map<int, Media> mMedia; // by m-line index

	// Candidates
	std::vector<Candidate> mCandidates;
	bool mEnded = false;

	static Type stringToType(const string &typeString);
	static string typeToString(Type type);
	static string roleToString(Role role);
};

} // namespace rtc

std::ostream &operator<<(std::ostream &out, const rtc::Description &description);

#endif
