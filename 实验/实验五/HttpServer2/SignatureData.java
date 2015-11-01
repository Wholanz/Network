/**
 * Created by LK on 2015/6/16.
 */
/**
 * Created by LK on 2015/6/15.
 */
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.spec.PKCS8EncodedKeySpec;

public class SignatureData {


    public byte[] run(PrivateKey prikey, String myinfo) {
        try {
//            String prikeyvalue = "30820277020100300d";//这是GenerateKeyPair输出的私钥编码
//
//            Base64.Encoder encoder = Base64.getEncoder();

            PKCS8EncodedKeySpec priPKCS8 = new PKCS8EncodedKeySpec(prikey.getEncoded());
            KeyFactory keyf = KeyFactory.getInstance("RSA");


            PrivateKey myprikey = keyf.generatePrivate(priPKCS8);

//            String myinfo = "orderId=10dkfadsfksdkssdkd&amount=80&orderTime=20060509"; // 要签名的信息
// 用私钥对信息生成数字签名
            java.security.Signature signet = java.security.Signature
                    .getInstance("MD5withRSA");
            signet.initSign(myprikey);
            signet.update(myinfo.getBytes("ISO-8859-1"));
            byte[] signed = signet.sign(); // 对信息的数字签名

            System.out.println("signed(签名内容)原值=" + bytesToHexStr(signed));
            System.out.println("info（原值）=" + myinfo);

            System.out.println("签名并生成文件成功");
            return signed;
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("签名并生成文件失败");
        }
        ;
        return null;
    }

    /**
     * Transform the specified byte into a Hex String form.
     */
    public static final String bytesToHexStr(byte[] bcd) {
        StringBuffer s = new StringBuffer(bcd.length * 2);

        for (int i = 0; i < bcd.length; i++) {
            s.append(bcdLookup[(bcd[i] >>> 4) & 0x0f]);
            s.append(bcdLookup[bcd[i] & 0x0f]);
        }

        return s.toString();
    }

    /**
     * Transform the specified Hex String into a byte array.
     */
    public static final byte[] hexStrToBytes(String s) {
        byte[] bytes;

        bytes = new byte[s.length() / 2];

        for (int i = 0; i < bytes.length; i++) {
            bytes[i] = (byte) Integer.parseInt(s.substring(2 * i, 2 * i + 2),
                    16);
        }

        return bytes;
    }

    private static final char[] bcdLookup = {'0', '1', '2', '3', '4', '5',
            '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

}